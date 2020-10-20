// Define the div for the tooltip
var div = d3.select("body").append("div")	
    .attr("class", "tooltip")				
    .style("opacity", 0);


d3.json("https://raw.githubusercontent.com/giovannidiana/springraph/master/ligand_receptors_19102020/exported_data.JSON", 
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
            .attr("fill","none")
            .attr("stroke-width",0.2);

        d3.select("svg")
            .selectAll("label_cell")
            .data(data.cell_df)
            .enter()
            .append("text")
            .attr("class","cell")
            .attr("font-family", "sans-serif")
            .attr("font-size", ".5px")
            .attr("x",function(d){return d.x+(6+d.radius)*Math.cos(d.angle);})
            .attr("y",function(d){return d.y+(6+d.radius)*Math.sin(d.angle);})
            .text(function(d){return d.name;})

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

        var GNODE=d3.select("svg")
            .selectAll("g_node")
            .data(data.node_df)
            .enter()
            .append("g")
            .attr("class","node")
            .attr("id",function(d){return "node"+d.node;})
            .on("mouseenter", on_mouseover)
            .on("mouseleave", on_mouseout);

        var rect_wdt=0.6;
        GNODE.each(function(d,i){
            if(d.node_type==="R"){
                d3.select(this).append("rect")
                    .attr("class","marker")
                    .attr("x",function(d){ return d.x-rect_wdt/2.0;})
                    .attr("y",function(d){ return d.y-rect_wdt/2.0;})
                    .attr("width",rect_wdt)
                    .attr("height",rect_wdt)
                    .attr("stroke-width", 0.1)
                    .attr("stroke","white")
                    .attr("fill", function(d){ return d.color;})
            } else {
                d3.select(this).append("circle")
                    .attr("class","marker")
                    .attr("cx",function(d){ return d.x;})
                    .attr("cy",function(d){ return d.y;})
                    .attr("r",0.3)
                    .attr("stroke-width", 0.1)
                    .attr("stroke","white")
                    .attr("fill", function(d){ return d.color;})
            }
        });

        GNODE.append("text")
        .attr("x",function(d){return d.x+rect_wdt/2})
        .attr("y",function(d){return d.y+rect_wdt})
        .style("font","0.3px sant-serif" )
        .text(function(d){return d.name;})

        var currentGroup = "ALL";
        var key_list = Object.keys(data.edge_pathways[0])
        key_list.push("ALL");
        key_list.push("UP");
        key_list.push("DOWN");

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

                let bool_var;

                if(g=="UP" || g=="DOWN"){
                    bool_var = (data.edge_df[ind].updown==g);
                } else {
                    bool_var = (data.edge_pathways[ind][g]===1);
                }

                if(bool_var || g=="ALL"){
                    d3.select("#node"+data.edge_df[ind].from)
                        .style("display","inline")
                    d3.select("#node"+data.edge_df[ind].to)
                        .style("display","inline")

                    d3.select("#edge"+data.edge_df[ind].index)
                        .style("display","inline")

                }
            }

        }
    });

function on_mouseover(d,i){
    let marker = d3.select(this).select(".marker");
    marker.attr("fill","orange");
    d3.select(this).select("text").attr("visibility","hidden");
    div.transition()
    .duration(200)
    .style("opacity",0.9)
    console.log(d.name)
    div.html(d.name)
    .style("left",d3.event.pageX+"px")
    .style("top",d3.event.pageY-20+"px")
}

function on_mouseout(d,i){
    d3.select(this).select(".marker").attr("fill",function(d) {return d.color;});
    d3.select(this).select("text").attr("visibility","visible");
    div.transition().style("opacity",0);
}


