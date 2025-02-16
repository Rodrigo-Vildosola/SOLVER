# Solver Notes

I get now how we will use an AST to enhance the library, we will on the conversion to AST, 
convert each type of node and operator into an AST Node that stores each ones types, 
the children and also a callback for each operator, this way if we call the callback 
of the root node it should evaluate the results of the whole expression
