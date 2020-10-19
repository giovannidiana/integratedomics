          d3.json("https://raw.githubusercontent.com/giovannidiana/springraph/master/ligand_receptors_13102020/exported_data.JSON", function(data) {

              d3.select("svg")
                .selectAll("circle_cell")
                .data(data.cell_df)
                .enter()
                .append("circle").attr("class","cell")
                                 .attr("cx",function(d){return d.x;})
                                 .attr("cy",function(d){return d.y;})
                                 .attr("r",function(d){return d.radius;})
                                 .attr("stroke","grey")
                                 .attr("fill","white")
                                 .attr("stroke-width",0.2)

              d3.select("svg")
                .selectAll("line")
                .data(data.edge_df)
                .enter()
                  .append("line").attr("x1",function(d){ return d.x1;})
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
                  .attr("cx",function(d){ return d.x;})
                  .attr("cy",function(d){ return d.y;})
                                 .attr("r",0.3)
                  .attr("stroke-width", 0.1)
                  .attr("stroke","white")
                  .attr("fill", function(d){ return d.color;})
                .on("mouseover", on_mouseover)
                .on("mouseout", on_mouseout)

              var rect_wdt=0.6;

              nodeR = data.node_df.filter(function(d){return d.node_type=='R';})
              d3.select("svg")
             .selectAll("rect")
              .data(nodeR)
              .enter()
                  .append("rect")
                  .attr("x",function(d){ return d.x-rect_wdt/2.0;})
                  .attr("y",function(d){ return d.y-rect_wdt/2.0;})
                  .attr("width",rect_wdt)
                  .attr("height",rect_wdt)
                  .attr("stroke-width", 0.1)
                  .attr("stroke","white")
                  .attr("fill", function(d){ return d.color;})
                .on("mouseover", on_mouseover)
                .on("mouseout", on_mouseout)

          
              d3.select("svg")
               .selectAll("text")
                .data(data.node_df)
                .enter()
                .append("text")
                .attr("id",function(d){
                            if(d.cell=='-1'){
                                return d.node;
                            } else {
                                console.log(d.node+d.cell);
                                return d.node+d.cell;
                                
                            }})
                .attr("font-family", "sans-serif")
                .attr("font-size", ".3px")
                .attr("x",function(d){return d.x+rect_wdt/2;})
                .attr("y",function(d){return d.y+rect_wdt/2;})
                  .text(function(d){return d.node;})
                            


          });

          function on_mouseover(d,i){
              d3.select(this).attr("fill","orange");
              var nodename = d.node;
              if(d.cell!=-1){
                  nodename=nodename+d.cell;
              }
              console.log(nodename)

              d3.select("#"+nodename).attr("font-size","1px");
          }
          
            function on_mouseout(d,i){
                d3.select(this).attr("fill",function(d) {return d.color;});
                var nodename = d.node;
                if(d.cell!=-1) nodename+=d.cell;
                d3.select("#"+nodename).attr("font-size",".3px");
          }
