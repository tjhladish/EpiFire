layout <- read.csv("layout_layout.csv", header=F)
edges  <- read.csv("layout.csv", header=F)
with(layout, plot(V2,V3, pch=20))
apply(edges, 1, function(x) { with(rbind(layout[x[1]+1, -1],layout[x[2]+1, -1]), lines(V2,V3)) })