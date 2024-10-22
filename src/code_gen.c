

void AssemblyGenerateExpression(file File, ast_node * Node);
void AssemblyGenerateFactor(file File,  factor Factor);

void AssemblyGenerateBinaryOperator(file File, binary_operator Operator){
    AssemblyGenerateExpression(File, Operator.Left);
    FileWriteFormatted(File, "\tpush eax\n");
    AssemblyGenerateExpression(File, Operator.Right);
    FileWriteFormatted(File, "\tpop ecx\n");
    switch(Operator.Enum){
        case BINARY_OPERATOR_TYPE_ADD:{
            FileWriteFormatted(File, "\tadd eax, ecx\n");
        }break;

        case BINARY_OPERATOR_TYPE_MUL:{
            FileWriteFormatted(File, "\timul eax, ecx\n");
        }break;
        
        case BINARY_OPERATOR_TYPE_MIN:{
            FileWriteFormatted(File, "\tsub ecx, eax\n");
            FileWriteFormatted(File, "\tmov eax, ecx\n");
        }break;

        case BINARY_OPERATOR_TYPE_DIV:{
            FileWriteFormatted(File, "\txchg ecx, eax\n");
            FileWriteFormatted(File, "\tcdq\n");
            FileWriteFormatted(File, "\tidiv ecx\n");
        }break;
        
        
    }
}


void AssemblyGenerateUnaryOperator(file File, unary_operator Operator){
    AssemblyGenerateFactor(File, *Operator.Factor);
            
    switch(Operator.Enum){
        case UNARY_OPERATOR_TYPE_NEGATION:{
            FileWriteFormatted(File, "\tneg eax\n");
        }break;
                
        case UNARY_OPERATOR_TYPE_BIT_COMPLEMENT:{
            FileWriteFormatted(File, "\tnot eax\n");
        }break;

        case UNARY_OPERATOR_TYPE_LOG_NEGATION:{
            FileWriteFormatted(File, "\tcmp eax, 0\n");
            FileWriteFormatted(File, "\tmov eax, 0\n");
            FileWriteFormatted(File, "\tsete al\n");
        }break;
    }
}


void AssemblyGenerateFactor(file File,  factor Factor){
    switch(Factor.Enum){
        case FACTOR_TYPE_EXPRESSION:{
            AssemblyGenerateExpression(File, Factor.Expression);
        }break;
        
        case FACTOR_TYPE_NUMBER:{
            FileWriteFormatted(File, "\tmov eax, %i\n", Factor.Number);
        }break;

        case FACTOR_TYPE_UNARY_OP:{
            AssemblyGenerateUnaryOperator(File, Factor.UnOp);
        }break;
    }
    
}

void AssemblyGenerateExpression(file File, ast_node * Node){
    switch(Node->Expression.Enum){
        case EXPRESSION_TYPE_FACTOR:{
            AssemblyGenerateFactor(File, Node->Expression.Factor);
        }break;
        case EXPRESSION_TYPE_OPERATOR:{
            AssemblyGenerateBinaryOperator(File, Node->Expression.Operator);
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
