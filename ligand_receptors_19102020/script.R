
data<-read.table("secretome_sheet1.csv",header=T)
pathways<-read.table("secretome_sheet2.csv",header=T)
edge_pathways<-data.frame()

process=unique(sort(data$Ligand))
ligand_updown=unique(data[order(data$Ligand),c(1,3)])[,2]

# cell_receptor labels
cell_rec=c()
append=F

# find receptor_cell labels
for(i in 1:nrow(data)){
	for(j in 1:6){
		if(!is.na(data[i,3+j])){
			cell_rec=c(cell_rec,paste(data$Receptor[i],colnames(data)[3+j],sep="_"))
		}
	}
}

# remove duplicates
ord=order(cell_rec)
cell_rec=unique(sort(cell_rec))
receptor_updown=rep("DOWN",length(cell_rec))
#extract the cell type for each label
cell_rec_cell=mapply("[",strsplit(cell_rec,"_"),2)
cell_rec_rec=mapply("[",strsplit(cell_rec,"_"),1)

# generate list of receptor regulation
for(i in 1:length(cell_rec)){
	tmp=data[data[,2]==cell_rec_rec[i],4:9];
	if(any(tmp[[cell_rec_cell[i]]]=="UP" ,na.rm=T)) receptor_updown[i]="UP"
}

# remove duplicates (this should just be the list of 7)
celltypes=unique(sort(cell_rec_cell))
# scaling for cell positioning
scaling=10
# how spread receptors within cells are
rad=rep(NA,6)
# positions of receptors
cell_rec_pos=matrix(NA,length(cell_rec),2)
# how many receptors per cell
recxcell=rep(NA,6)
cell_angle=rep(NA,6)
for(i in celltypes){
	index=which(celltypes==i)
	recxcell[index]=sum(cell_rec_cell==i)
}

# cell positions
ctpos=matrix(NA,6,2)
for(i in celltypes){
	index=which(i==celltypes)
	cell_angle[index]= 2*pi * (cumsum(recxcell)[index]-0.5*recxcell[index])/sum(recxcell)
    ctpos[index,]=scaling*c(cos(cell_angle[index]),sin(cell_angle[index]))
}

#assign x/y for each receptor
for(i in celltypes){
	k=which(i==celltypes)
	tot=recxcell[k]
	rad[k] = tot/4.5;
	
	cell_rec_pos[cell_rec_cell==i,1] = ctpos[k,1]+rad[k]*cos(0:(tot-1)*2*pi/tot)
	cell_rec_pos[cell_rec_cell==i,2] = ctpos[k,2]+rad[k]*sin(0:(tot-1)*2*pi/tot)
}

# append on input.dat the edges
for(i in 1:nrow(data)){
	for(j in 1:6){
		if(!is.na(data[i,3+j])){
			write(paste(which(paste(data$Receptor[i],colnames(data)[3+j],sep="_")==cell_rec),which(data$Ligand[i]==process),1),file="input.dat",append=append)
			write(as.character(data[i,3+j]),file="edge_regulation.dat",append=append)
			edge_pathways=rbind(edge_pathways,pathways[i,])

			append=TRUE
		}
	}
}

#write receptor_cell labels into labels.txt
write.table(cell_rec,file="labels.txt",col.names=F,row.names=F)
write.table(cell_rec_rec,file="labels_rec.txt",col.names=F,row.names=F)
write.table(as.numeric(as.factor(cell_rec_cell)),file="labels_cell.txt",col.names=F,row.names=F)
# write ligand labels in process_labels.
write.table(process,file="process_labels.txt",col.names=F,row.names=F)
# write receptor_cell positions 
write.table(cell_rec_pos,file="receptors_positions.txt",col.names=F,row.names=F)
#write cell radii
write.table(rad,file="cell_rad.txt",col.names=F,row.names=F)
write.table(cell_angle,file="cell_angle.txt",col.names=F,row.names=F)

# write ligand state
write.table(ligand_updown, file="ligand_updown.txt",col.names=F,row.names=F)
write.table(receptor_updown, file="receptor_updown.txt",col.names=F,row.names=F)

names(edge_pathways) = gsub("\\.",' ',names(edge_pathways))
write.table(edge_pathways, file="edge_pathways.txt",col.names=T,row.names=F)
