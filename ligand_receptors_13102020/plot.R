library(igraph)
library(plotrix)

folder="."
LO<-as.matrix(read.table(paste(folder,"/graph.dat",sep="")))
nodes_rec <- as.character(read.table(paste(folder,"/labels_rec.txt",sep=""))$V1)
nodes_lig <- as.character(read.table(paste(folder,"/process_labels.txt",sep=""))$V1)
nodes <- 1:(length(nodes_rec)+length(nodes_lig))
rad <- read.table(paste(folder,"/cell_rad.txt",sep=""))$V1
cell_name<-c("CD31", "COL-HIGH", "COL-LOW", "NES-HIGH", "NES-LOW","NG2")
cell_angle <- read.table(paste(folder,"/cell_angle.txt",sep=""))$V1
cell_ID <- read.table(paste(folder,"/labels_cell.txt",sep=""))$V1
node_type=rep("L",length(nodes))
node_type[1:length(nodes_rec)]="R"
ligand_updown <- as.character(read.table(paste(folder,"/ligand_updown.txt",sep=""))$V1)
receptor_updown <- as.character(read.table(paste(folder,"/receptor_updown.txt",sep=""))$V1)
edge_updown <- as.character(read.table(paste(folder,"/edge_regulation.dat",sep=""))$V1)
edge_pathways <- read.table(paste(folder,"/edge_pathways.txt",sep=""),header=T)
updown=c(receptor_updown,ligand_updown)

node_color=updown
node_color[updown=="UP"]="red"
node_color[updown=="DOWN"]="blue"

edge_color=edge_updown
edge_color[edge_updown=="UP"]=rgb(1,0,0,.3)
edge_color[edge_updown=="DOWN"]=rgb(0,0,1,.3)

textrot=rep(0,length(nodes))
for(i in 1:length(nodes_rec)){
  cell=cell_ID[i]-1
  theta=cell_angle[cell+1]
  cell_pos=c(10*cos(theta), 10*sin(theta))
  textrot[i] = atan2(LO[i,3]-cell_pos[2],LO[i,2]-cell_pos[1])
}

edgedata<-read.table(paste(folder,"/input.dat",sep=""))
edges<- data.frame(from=edgedata[,1], to=edgedata[,2]+length(nodes_rec), color=edge_color)

node_select=list(UP = (updown=="UP"),
                 DOWN = (updown=="DOWN"))
edge_select=list(UP = (edge_updown=="UP"),
                 DOWN = (edge_updown=="DOWN"))

proc = vector("list",ncol(edge_pathways))
names(proc) = colnames(edge_pathways)

 input=list()
 input$type="ALL"
  
    if(input$type=="ALL"){
      filter_node=rep(TRUE,length(nodes))
      filter_edge=rep(TRUE,nrow(edges))
    } else if(input$type=="UP" || input$type=="DOWN"){
      filter_node = node_select[[input$type]]
      filter_edge = edge_select[[input$type]]
    } else {
      filter_edge = edge_pathways[[input$type]]==1
      filter_node = (nodes %in% edges$to[filter_edge]) | (nodes %in% edges$from[filter_edge])
    }
    net=graph_from_data_frame(edges[filter_edge,],vertices=nodes[filter_node],directed=F)
    V(net)$label=c(nodes_rec,nodes_lig)[filter_node]
    V(net)$label.dist = 8
    V(net)$label.degree = -pi/2
    #V(net)$size=c(rep(5,length(nodes_rec)),rep(2,length(nodes_lig)))
    V(net)$color = node_color[filter_node]
    V(net)$shape = c(rep("square",length(nodes_rec)),rep("circle",length(nodes_lig)))[filter_node]
    #E(net)$color = edge_updown 
    #E(net)$width = 10
    if(TRUE){#input$password=="delta2019") {
      
      plot(net,layout=LO[filter_node,2:3],rescale=F,xlim=c(-12,12),ylim=c(-14,14),
         vertex.size = 40)
    
    #for (i in 1:length(nodes)) {
    #	text(x=LO[i,1], y=LO[i,2], labels=c(nodes_rec,nodes_lig)[i], adj=NULL, pos=NULL, cex=.7, col="black", srt=180/pi*textrot[i], xpd=T)
    #}
    
	  for(i in 1:7) draw.circle(x=10*cos(cell_angle[i]),y=10*sin(cell_angle[i]),radius=rad[i])
	  text(x=17*cos(cell_angle),y=17*sin(cell_angle),labels=cell_name)
	  
    }



### create JSON file
library(jsonlite)
datalist = list(
                node_df=data.frame(
                        node =LO[,1], 
                        name = c(nodes_rec,nodes_lig), 
                        node_type=node_type,
                        cell= c(cell_name[cell_ID],rep(-1,length(nodes_lig))),
                        x=LO[,2],
                        y=LO[,3],
                        color=node_color),
                edge_df=data.frame(
                        index=1:nrow(edgedata)-1,
                        from=nodes_rec[edgedata[,1]], 
                        to=nodes_lig[edgedata[,2]], 
                        x1=LO[edgedata[,1],2],
                        x2=LO[edgedata[,2]+length(nodes_rec),2],
                        y1=LO[edgedata[,1],3],
                        y2=LO[edgedata[,2]+length(nodes_rec),3],
                        color=edge_color),
                edge_pathways=edge_pathways,
                cell_df=data.frame(
                        x = 10*cos(cell_angle),
                        y = 10*sin(cell_angle),
                        angle=cell_angle,
                        radius = rad,
                        name = cell_name)

                )

js = toJSON(datalist)
write(js,file="exported_data.JSON")

