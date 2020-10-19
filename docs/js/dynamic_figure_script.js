d3.json("https://raw.githubusercontent.com/giovannidiana/springraph/master/ligand_receptors_13102020/exported_data.JSON", 
    function(data) {
        d3.select("svg")
            .selectAll("circle_cell")
            .data(data.cell_df)
            .enter()
            .append("circle") 
            .attr("class","cell")
            .attr("cx",function(d){return d.x;})
            .attr("cy",function(d){return d.y;})
            .attr("r",function(d){return d.radius;})
            .attr("stroke","grey")
            .attr("fill","white")
            .attr("stroke-width",0.2);

        d3.select("svg")
            .selectAll("line")
            .data(data.edge_df)
            .enter()
            .append("line")
            .attr("id",function(d){ return "edge"+d.index;})
            .attr("x1",function(d){ return d.x1;})
            .attr("x2",function(d){ return d.x2;})
            .attr("y1",function(d){ return d.y1;})
            .attr("y2",function(d){ return d.y2;})
            .attr("stroke-width",0.1)
            .attr("stroke",function(d){return d.color;});

        nodeL = data.node_df.filter(function(d){return d.node_type=='L';})
        d3.select("svg")
            .selectAll("circle_node")
            .data(nodeL)
            .enter()
            .append("circle").attr("class","node")
            .attr("id",function(d){ return "node"+d.node;})
            .attr("cx",function(d){ return d.x;})
            .attr("cy",function(d){ return d.y;})
            .attr("r",0.3)
            .attr("stroke-width", 0.1)
            .attr("stroke","white")
            .attr("fill", function(d){ return d.color;})
            .on("mouseenter", on_mouseover)
            .on("mouseleave", on_mouseout)

        var rect_wdt=0.6;

        nodeR = data.node_df.filter(function(d){return d.node_type=='R';})
        d3.select("svg")
            .selectAll("rect_node")
            .data(nodeR)
            .enter()
            .append("rect")
            .attr("class","node")
            .attr("id",function(d){ return "node"+d.node;})
            .attr("x",function(d){ return d.x-rect_wdt/2.0;})
            .attr("y",function(d){ return d.y-rect_wdt/2.0;})
            .attr("width",rect_wdt)
            .attr("height",rect_wdt)
            .attr("stroke-width", 0.1)
            .attr("stroke","white")
            .attr("fill", function(d){ return d.color;})
            .on("mouseenter", on_mouseover)
            .on("mouseleave", on_mouseout)


        d3.select("svg")
            .selectAll("text_node")
            .data(data.node_df)
            .enter()
            .append("text")
            .attr("class","node")
            .attr("id",function(d){ return "text"+d.node;})
            .attr("font-family", "sans-serif")
            .attr("font-size", ".3px")
            .attr("x",function(d){return d.x+rect_wdt/2;})
            .attr("y",function(d){return d.y+rect_wdt;})
            .text(function(d){return d.node;})

        var currentGroup = "ALL";
        var key_list = Object.keys(data.edge_pathways[0])
        key_list.push("ALL");

        var dropDown = d3.select("#dropdown_pathways")
            .selectAll("option")
            .data(key_list)
            .enter()
            .append('option')
            .text(function(d) {return d;})
            .attr("value", function(d) { return d; })
            .property("selected",function(d) { return d===currentGroup;});

        d3.select("#dropdown_pathways")
            .on("change",
                function(d){
                    currentGroup = this.value;
                    onSelectionChange(currentGroup)
                });

        function onSelectionChange(g){
            d3.select("svg").selectAll(".node")
                .style("display","none")
            d3.select("svg").selectAll("line")
                .style("display","none")

            for(let ind=0;ind<data.edge_df.length;ind++){
                if(data.edge_pathways[ind][g]===1){
                    d3.select("#node"+data.edge_df[ind].from)
                        .style("display","inline")
                    console.log("#node"+data.edge_df[ind].from)
                    d3.select("#node"+data.edge_df[ind].to)
                        .style("display","inline")
                    d3.select("#edge"+data.edge_df[ind].index)
                        .style("display","inline")                  
                }
            }

        }
    });

function on_mouseover(d,i){
    d3.select(this).attr("fill","orange");
    d3.select("#text"+d.node).transition().attr("font-size","1px");
}

function on_mouseout(d,i){
    d3.select(this).attr("fill",function(d) {return d.color;});
    d3.select("#text"+d.node).attr("font-size",".3px");
}


