#ifndef COMPILER_H
#define COMPILER_H

class Compiler
{
	public:
		Compiler();

		virtual void compiler(istream& input,ByteCodeStream& output)
		{
			Scanner scanner(intput);
			ProgramNodeBuilder builder;
			Parser parser;

			parser.Parse(scanner,builder);

			RISCCodeGenerator generator(output);
			ProgramNode* parseTree=builder.getRootNde();
			parseTree->Traverse(generator);
		}
};

#endif
