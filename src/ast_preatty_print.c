void AstPrintExpression(ast_node * Node){
}


void AstPrintStatement(ast_node * Node){
    switch(Node->Statement.Enum){
        case STATEMENT_TYPE_RETURN:{
            ast_node * ExpressionNode = Node->Statement.Return.Expression;
            //printf("\tRETURN s32<%i>", ExpressionNode->Expression.Number);
        }break;
    }
}

void AstPrint(ast * Ast){
    For(FunctionNode, Ast->Functions){
        printf("Func s32 %s:\n", FunctionNode->Function.Id.Literal);
        AstPrintStatement(FunctionNode->Function.Statement);
        printf("\n\n");
    }
}
