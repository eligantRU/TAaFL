@ECHO OFF   

GraphVisualizer.exe < %1
dot -T png -o result.png graphizFormatedGraph.dot 
