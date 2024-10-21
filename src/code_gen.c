
void AssemblyGenerateExpression(file File, ast_node * Node);

void AssemblyGenerateOperator(file File, operator Operator){
    switch(Operator.Enum){
        case OPERATOR_TYPE_UNARY:{
            unary_operator UOps = Operator.UnaryOperator;
            AssemblyGenerateExpression(File, UOps.Expression);
            
            switch(UOps.Enum){
                case UNARY_OPERATOR_TYPE_NEGATION:
                case UNARY_OPERATOR_TYPE_BIT_COMPLEMENT:{
                    FileWriteFormatted(File, "\tneg eax\n");
                }break;

                case UNARY_OPERATOR_TYPE_LOG_NEGATION:{
                    FileWriteFormatted(File, "\tneg eax\n");
                }break;
            }
        }break;
    }
}


void AssemblyGenerateExpression(file File, ast_node * Node){
    switch(Node->Expression.Enum){
        case EXPRESSION_TYPE_NUMBER_LITERAL:{
            FileWriteFormatted(File, "\tmov eax, %i\n", Node->Expression.Number);
        }break;
        case EXPRESSION_TYPE_OPERATION:{
            AssemblyGenerateOperator(File, Node->Expression.Operator);
        }break;
    }
}

void AssemblyGenerateStatement(file File, ast_node * Node){
    switch(Node->Statement.Enum){
        case STATEMENT_TYPE_RETURN:{
            AssemblyGenerateExpression(File, Node->Statement.Return.Expression);
            FileWriteFormatted(File, "\tret\n");
        }break;
    }
}

void AssemblyGenerate(ast* Ast, char  * OutputPath){
    file File = FileOpen(OutputPath, "w+");

    FileWriteFormatted(File, ".intel_syntax\n");
    
    For(FunctionNode, Ast->Functions){
        FileWriteFormatted(File, ".global _%s\n", FunctionNode->Function.Id.Literal);
        FileWriteFormatted(File, "_%s:\n", FunctionNode->Function.Id.Literal);
        
        AssemblyGenerateStatement(File, FunctionNode->Function.Statement);
    }
    FileClose(File);
    

    
}
