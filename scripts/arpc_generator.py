import os
import random
import shutil
import glob
import sys


class ArpcStruct:
    def __init__(self, struct_string):
        self.rawStruct = struct_string
        self.struct_string = struct_string
        self.struct_name = None
        self.variables = []
        self.preprocess()
        self.parse()

    def preprocess(self):
        # Remove ARPC_STRUCT macro
        self.rawStruct = self.struct_string = self.struct_string.replace(
            "ARPC_STRUCT", "")
        # Remove comments
        while "/*" in self.struct_string:
            start = self.struct_string.find("/*")
            end = self.struct_string.find("*/", start + 2)
            if end != -1:
                self.struct_string = self.struct_string[:start] + \
                    self.struct_string[end + 2:]

        # Find the start and end positions of the struct definition
        struct_start = self.struct_string.find("typedef struct")
        brace_start = self.struct_string.find("{", struct_start)
        brace_end = self.struct_string.rfind("}", brace_start)

        if struct_start != -1 and brace_start != -1 and brace_end != -1:
            typedef_name_start = self.struct_string.find("}", brace_end) + 1
            typedef_name_end = self.struct_string.find(";", typedef_name_start)
            self.struct_name = self.struct_string[typedef_name_start:typedef_name_end].strip(
            )
            self.struct_string = self.struct_string[brace_start:brace_end + 1]
        else:
            raise ValueError(
                "Invalid struct format or not found: ", self.struct_string)

    def parse(self):
        # Extract variables and their types
        variables_section = self.struct_string[self.struct_string.find(
            "{") + 1: self.struct_string.rfind("}")]
        variables_list = [var.strip()
                          for var in variables_section.split(";") if var.strip()]
        for variable in variables_list:
            var_parts = variable.split()
            if len(var_parts) >= 2:
                if "*" in variable:
                    raise Exception("Pointers are not supported")
                var_type = ' '.join(var_parts[:-1])
                var_name = var_parts[-1].rstrip(";")
                self.variables.append((var_type.strip(), var_name.strip()))
        print("Parsed struct: ", self.struct_name, self.variables)
        globalStructs.append(self)

    def get_struct_name(self):
        return self.struct_name

    def get_variables(self):
        return self.variables

    def getRawStruct(self):
        return self.rawStruct


class ArpcFunction:
    acceptedTypes = [
        "void",
        "int8_t", "uint8_t",
        "int16_t", "uint16_t",
        "int32_t", "uint32_t",
        "int64_t", "uint64_t",
        "float", "double"
    ]

    rawFunctionDecleration = ""

    returnType = ""
    functionName = ""
    parameters = []

    parametersUnwrapped = []

    structs: ArpcStruct = []

    def __init__(self, rawFunctionDecleration, functionId):
        self.rawFunctionDecleration = rawFunctionDecleration
        self.functionId = functionId
        self.parametersUnwrapped = []
        self.structs = []

        self.preprocess()
        self.parse()

        print(
            f"Registered function {self.__str__()} with id {self.functionId}")

    def __str__(self):
        return f"{self.returnType} {self.functionName}({', '.join([f'{paramType} {paramName}' for paramType, paramName in self.parameters])})"

    def isStruct(self, parameterType):
        for struct in globalStructs:
            if struct.get_struct_name() == parameterType:
                return True
        return False

    def checkParameter(self, parameterType):
        if self.isStruct(parameterType):
            return True
        if parameterType in self.acceptedTypes:
            return True
        return False

    def preprocess(self):
        # if there is '_rpc' in the declaration, remove it
        if "RPC" in self.rawFunctionDecleration:
            self.rawFunctionDecleration = self.rawFunctionDecleration.replace(
                "RPC", '')

        # Remove leading and trailing whitespace
        self.rawFunctionDecleration = self.rawFunctionDecleration.strip()

    def parse(self):
        # Split the declaration by whitespace
        parts = self.rawFunctionDecleration.split()

        # Extract the return type
        self.returnType = parts[0]

        if not self.checkParameter(self.returnType):
            raise Exception(f"Invalid return type: {self.returnType}")

        # Extract the function name
        self.functionName = parts[1].split('(')[0]

        # Extract the parameter section
        startIndex = self.rawFunctionDecleration.find('(')
        endIndex = self.rawFunctionDecleration.rfind(')')
        parameterSection = self.rawFunctionDecleration[startIndex + 1: endIndex]

        self.parameters = []

        # Check if there are no parameters
        if parameterSection.strip() != "":
            # Split the parameter section by commas
            parametersRaw = parameterSection.split(',')

            # Extract the parameter types and names
            for parameter in parametersRaw:
                parameter = parameter.strip()
                parameterParts = parameter.split()
                parameterType = ' '.join(
                    parameterParts[:-1])
                subParameterType = parameterType
                parameterName = parameterParts[-1]
                subParameterName = parameterName

                if self.checkParameter(parameterType):
                    if self.isStruct(parameterType):
                        # add all members of the struct to the parameters
                        for struct in globalStructs:
                            if struct.get_struct_name() == parameterType:
                                self.structs.append(struct)
                                for variable in struct.get_variables():
                                    subParameterType, subParameterName = variable
                                    subParameterName = parameterName + "." + subParameterName
                                    self.parametersUnwrapped.append(
                                        (subParameterType, subParameterName))
                                break

                    self.parameters.append((parameterType, parameterName))
                    self.parametersUnwrapped.append(
                        (subParameterType, subParameterName))

                else:
                    raise Exception(
                        f"Invalid parameter type: {parameterType}")

    def generateClientCode(self):
        clientCode_c = []
        clientCode_h = []

        # CALL FRAME
        functionIdDefine = f"\n#define {self.functionName}_ID {self.functionId}\n"
        clientCode_c.append(functionIdDefine)

        for struct in self.structs:
            clientCode_h.append(struct.getRawStruct())
            clientCode_c.append(struct.getRawStruct())

        functionDeclaration = f"void {self.functionName}_generateCallFrame(arpcDataFrame_t *frame,"
        hasParameters = self.parameters != []
        parameterDeclarations = hasParameters * ', '.join([f"{paramType} {paramName}" for paramType,
                                                           paramName in self.parameters])
        clientCode_c.append(functionDeclaration +
                            parameterDeclarations + ")\n{\n")

        if hasParameters:
            # Calculate the size of the parameters in bytes
            parameterSizeInBytes = ' + '.join(
                [f"sizeof({paramName})" for _, paramName in self.parameters])
            parameterFrameDefine = f"uint8_t parameters[{parameterSizeInBytes}] = {{ 0 }};\n"
            clientCode_c.append(parameterFrameDefine)

            offset = ''
            parameterSerializationList = []
            for _, paramName in self.parametersUnwrapped:
                parameterSerializationList.append(
                    f"memcpy(parameters{offset}, &{paramName}, sizeof({paramName}));")
                offset += f"+ sizeof({paramName})"
            clientCode_c.append(
                '\n'.join(parameterSerializationList) + '\n')

            clientCode_c.append(
                f"arpcEncodeGeneric(frame, {self.functionName}_ID, parameters, sizeof(parameters));\n")
        else:
            clientCode_c.append(
                f"arpcEncodeGeneric(frame, {self.functionName}_ID, NULL, 0);\n")

        clientCode_c.append('}\n\n')

        # STUB
        functionDefinition = f"{self.returnType} {self.functionName}(" + \
            parameterDeclarations + ')'
        clientCode_h.append(functionDefinition + ';\n')
        clientCode_c.append(functionDefinition + '{\n')

        frameDefinitions = f"arpcDataFrame_t callFrame = {{ 0 }};\n"
        frameDefinitions += f"arpcDataFrame_t responseFrame = {{ 0 }};\n\n"
        clientCode_c.append(frameDefinitions)

        generateCallFrameCode = f"{self.functionName}_generateCallFrame(&callFrame" + (
            hasParameters * ', ' + ', '.join([f"{paramName}" for _, paramName in self.parameters])) + ');\n\n'
        clientCode_c.append(generateCallFrameCode)

        sendReceiveCode = f"arpcSendReceiveFrame(&callFrame, &responseFrame);\n\n"
        clientCode_c.append(sendReceiveCode)

        returnValueDeserializationCode = ''
        returnCode = ''
        if self.returnType != 'void':
            returnValueDefine = f"{self.returnType} returnValue = 0;\n"
            returnValueDeserialization = f"memcpy(&returnValue, responseFrame.parameters, sizeof({self.returnType}));\n\n"
            returnValueDeserializationCode += returnValueDefine + returnValueDeserialization
            returnCode = 'return returnValue;\n'
        clientCode_c.append(returnValueDeserializationCode)
        clientCode_c.append(returnCode)

        clientCode_c.append('}\n\n')

        return clientCode_h, clientCode_c

    def generateServerCode(self):
        serverCode = []

        functionIdDefine = f"\n#define {self.functionName}_ID {self.functionId}\n"
        serverCode.append(functionIdDefine)

        for struct in self.structs:
            serverCode.append(struct.getRawStruct())

        functionDecleration = f"extern {self.returnType} {self.functionName}("
        hasParameters = self.parameters != []
        parameterDeclarations = hasParameters * \
            ', '.join([f"{paramType} {paramName}" for paramType,
                      paramName in self.parameters]) + ');\n\n'
        serverCode.append(functionDecleration + parameterDeclarations)

        functionDefine = f"void {self.functionName}_generateResponseFrame(arpcDataFrame_t *callFrame, arpcDataFrame_t *responseFrame) {{\n"
        serverCode.append(functionDefine)

        if hasParameters:
            offset = ''
            for parameterType, parameterName in self.parameters:
                parameterDefine = f"{parameterType} {parameterName};\n"
                serverCode.append(parameterDefine)

            for parameterType, parameterName in self.parametersUnwrapped:
                parameterDeserialization = f"memcpy(&{parameterName}, callFrame->parameters{offset}, sizeof({parameterType}));\n"
                offset += f"+ sizeof({parameterType})"
                serverCode.append(parameterDeserialization)

        parameterDeclarations = ', '.join(
            [paramName for _, paramName in self.parameters])
        functionCall = f"{self.functionName}({parameterDeclarations});\n\n"

        if self.returnType != 'void':
            functionCall = f"{self.returnType} returnValue = " + functionCall
            serverCode.append(functionCall)

            returnValSerialisedDefine = f"uint8_t returnValueSerialised[sizeof({self.returnType})] = {{ 0 }};\n"
            serverCode.append(returnValSerialisedDefine)

            returnValueSerialization = f"memcpy(returnValueSerialised, &returnValue, sizeof({self.returnType}));\n"
            serverCode.append(returnValueSerialization)
            serverCode.append(
                f"arpcEncodeGeneric(responseFrame, {self.functionName}_ID, returnValueSerialised, sizeof(returnValueSerialised));\n\n")
        else:
            serverCode.append(functionCall)
            serverCode.append(
                f"arpcEncodeGeneric(responseFrame, {self.functionName}_ID, NULL, 0);\n\n")

        serverCode.append("}\n\n")

        return serverCode


class ArpcGenerator:
    outputDirectory = "../outputs/"
    generatedPathClient = "arpc_client/"
    generatedPathServer = "arpc_server/"

    constantsCode = []

    arpcFunctions: ArpcFunction = []

    def __init__(self, directory, outputDirectory):
        self.root = directory
        self.functionPrototypes = []
        self.constants = []

        self.outputDirectory = outputDirectory

        self.generatedPathClient = self.outputDirectory + self.generatedPathClient
        self.generatedPathServer = self.outputDirectory + self.generatedPathServer

        print("Output directory is: " + self.outputDirectory)

        self.cleanGeneratedFiles()

        os.mkdir(self.generatedPathClient)
        os.mkdir(self.generatedPathServer)

        self.copyFilesToGeneratedFolders()

        print("Looking for ARPC defines...")

        self.walkEveryLine()

        self.fillInFiles()

        self.formatFiles()

    def formatFiles(self):
        print("Formatting files...")
        os.system(f"clang-format -i {self.generatedPathClient}*.c")
        os.system(f"clang-format -i {self.generatedPathClient}*.h")
        os.system(f"clang-format -i {self.generatedPathServer}*.c")
        os.system(f"clang-format -i {self.generatedPathServer}*.h")

    def writeToFile(self, fileName, code):
        try:
            with open(fileName, 'a') as file:
                file.write(code)
        except FileNotFoundError:
            with open(fileName, 'w') as file:
                file.write(code)

    def fillInFiles(self):
        responseFrameAdressList = []

        print("Generating files...")

        self.writeToFile(self.generatedPathClient +
                         "arpc_client.h", "\n\n" + '\n'.join(self.constantsCode))

        for function in self.arpcFunctions:
            client_h, client_c = function.generateClientCode()
            server = function.generateServerCode()

            self.writeToFile(self.generatedPathClient +
                             "arpc_client.h", "\n\n" + '\n'.join(client_h))
            self.writeToFile(self.generatedPathClient +
                             "arpc_client.c", '\n'.join(client_c))
            self.writeToFile(self.generatedPathServer +
                             "arpc_server.c", '\n'.join(server))

            responseFrameAdressList.append(
                "&" + function.functionName + "_generateResponseFrame")

        responseFrameAddressesCode = "void (*const arpcFrameHandler[UINT8_MAX])(arpcDataFrame_t *callFrame, arpcDataFrame_t *responseFrame) = {" + ', '.join(
            responseFrameAdressList) + '};'
        self.writeToFile(self.generatedPathServer +
                         "arpc_server.c",  ''.join(responseFrameAddressesCode))

        self.writeToFile(self.generatedPathClient + "arpc_client.h",
                         "\n#ifdef __cplusplus\n}\n#endif\n#endif // ARPC_CLIENT_H\n")

    def copyFilesToGeneratedFolders(self):
        filesPath = os.getcwd() + "/templates/"
        genericFiles = ["arpc_generic.h", "arpc_generic.c"]
        filesClient = ["arpc_client.c", "arpc_client.h"]
        filesServer = ["arpc_server.c", "arpc_server.h"]

        for file in filesClient:
            shutil.copy(filesPath + file, self.generatedPathClient + file)

        for file in filesServer:
            shutil.copy(filesPath + file, self.generatedPathServer + file)

        for file in genericFiles:
            shutil.copy(filesPath + file, self.generatedPathClient + file)
            shutil.copy(filesPath + file, self.generatedPathServer + file)

    def cleanGeneratedFiles(self):
        paths = [self.generatedPathClient, self.generatedPathServer]
        for path in paths:
            if os.path.isfile(path) or os.path.islink(path):
                os.remove(path)  # remove the file
            elif os.path.isdir(path):
                shutil.rmtree(path)  # remove dir and all contains

    def functionSanityCheck(self, function):
        forbidden = ['*', '&', 'const', 'static', 'volatile', 'extern']

        if any(x in function for x in forbidden):
            return False
        else:
            return True

    def parseConstant(self, line):
        constant = line
        # get line inside bracets
        constant = constant[constant.find("(")+1:constant.find(")")]
        # remove comma
        constant = constant.replace(",", " ")
        # remove quotes
        constant = constant.replace("\"", "")

        self.constantsCode.append("#define " + constant)

        print("Found constant: " + constant)

    def isRPCFunction(self, line):
        line = line.strip()
        if line.startswith("RPC"):
            return True
        else:
            return False

    def isRPCConstant(self, line):
        if line.strip().startswith("ARPC_CONSTANT"):
            return True
        else:
            return False

    def isRPCStruct(self, line):
        line = line.strip()
        if "ARPC_STRUCT" in line:
            return True
        else:
            return False

    def walkEveryLine(self):
        files = []
        extensions = ['*.c', '*.cc', '*.cpp', '*.h', '*.hpp']

        functionId = 0

        os.chdir(self.root)
        for extension in extensions:
            for file in glob.glob(self.root+"/**/"+extension, recursive=True):
                files.append(file)

        struct_lines = []
        inside_struct = False

        lines = []

        # search all subdirs for files with the given extensions and find lines that start with the "RPC" and take until ')'. Function can be defined in multiple lines
        for file in files:
            filepath = os.path.join(self.root, file)
            with open(filepath, 'r') as f:
                for line in f:
                    lines.append(line)
                    if self.isRPCConstant(line):
                        self.parseConstant(line)
                    if self.isRPCStruct(line) or inside_struct:
                        if "struct" in line:
                            inside_struct = True
                        if inside_struct:
                            struct_lines.append(line)
                        if "}" in line and inside_struct:
                            inside_struct = False
                        if len(struct_lines) > 0 and not inside_struct:
                            globalStructs.append(
                                ArpcStruct(''.join(struct_lines)))
        for line in lines:
            if self.isRPCFunction(line):
                prototype = line
                if self.functionSanityCheck(prototype):
                    self.arpcFunctions.append(ArpcFunction(
                        prototype, functionId))
                    functionId += 1
                    if functionId == 255:
                        raise Exception("Too many functions")
                else:
                    raise Exception(
                        "Function prototype sanity check failed for function: " + prototype)


globalStructs: ArpcStruct = []

n = len(sys.argv)

if n != 3:
    print("Usage: python3 arpc_generator.py <path_to_search_directory> <output_directory> <rpc_indicator>")
    sys.exit(1)

searchDirectory = sys.argv[1]
outputDirectory = sys.argv[2]

ArpcGenerator(searchDirectory, outputDirectory)
