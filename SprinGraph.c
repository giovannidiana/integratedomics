#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>
#include<ctime>
#include<iostream>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include<vector>
#include<algorithm>
#include <tuple>
#include <fstream>

using namespace std;

const double R=5;
const double e0=0;
 double e11=1e-1;
const double e12=100e-1;
const double e22=100e-1;
const double SpringFactor=3;
double K_circle_bind=50;
const double noise = 1e-10;
double zoom=1.0;
double tx0=0,tx=0;
double ty0=0.0,ty=0;
const int W=700, H=700;
double L[4] = {-8,8,8,-8};
double delta[4]={-8,8,8,-8};
double dt=1e-2;
double cell_rad[7];
double cell_angle[7];

gsl_ran_discrete_t* gen=NULL;
gsl_rng * r = gsl_rng_alloc (gsl_rng_mt19937);

class Point {
    public:
     double x;
     double y;

     double dotprod(Point* p){
         return(x*p->x+y*p->y);
     }
     double norm(){
         return(sqrt( pow(x,2)+pow(y,2)));
     }
	 
	 double cosine(Point* p){
		 return(acos(dotprod(p)/(p->norm()*norm())));
	 }

	 double normdotprod(Point * p){
		 return(dotprod(p)/(norm()*p->norm()));
	 }

     Point rot(double theta){
         Point tmp;
         tmp.x=x*cos(theta)-y*sin(theta);
         tmp.y=x*sin(theta)+y*cos(theta);
         return(tmp);
     }
     double dist(Point* p){
         return(sqrt(pow(x-p->x,2)+pow(y-p->y,2)));
     }


};

class graph {
	public:
	void init(int n1,int n2);
	vector<int> NT1;
	vector<int> NT2;
	vector<Point> XY;
    vector<string> labels1;
    vector<string> labels2;
	vector<std::tuple<int,int,double> > EDGE;
	void add_edge(int n1,int n2, double w);
	void edges_from_table(char* filename);
	void load_labels1(char* filename);
	void load_labels2(char* filename);
	void load_init_pos1(char* filename);
	vector<int> which_cell;
	int ind;
};

graph G;

void graph::init (int n1,int n2){
	NT1.resize(n1);
	labels1.resize(n1);
	which_cell.resize(n1);
	NT2.resize(n2);
	labels2.resize(n2);
	XY.resize(n1+n2);
	for(unsigned int i=0; i<n1;++i){
		double theta=2.0*M_PI/n1*i;
		XY[i].x = R*cos(theta);
		XY[i].y = R*sin(theta);
		//cout<<XY[i].x<<' ' <<XY[i].y<<endl;
	}

	for(unsigned int i=n1; i<n1+n2;++i){
		XY[i].x = gsl_ran_gaussian(r,0.1);
		XY[i].y = gsl_ran_gaussian(r,0.1);
		//cout<<XY[i].x<<' ' <<XY[i].y<<endl;
	}
		
}

void graph::load_labels1(char* filename){
	ifstream input(filename);
	ifstream input_cell("labels_cell.txt");
	for(unsigned int i=0;i<NT1.size();i++){
		getline(input,labels1[i]);
		input_cell>>which_cell[i];
	}
	input.close();
	input_cell.close();
}

void graph::load_labels2(char* filename){
	ifstream input(filename);
	for(unsigned int i=0;i<NT2.size();i++){
		getline(input,labels2[i]);
	}
	input.close();
}

void graph::load_init_pos1(char* filename){
	ifstream input(filename);
	for(unsigned int i=0;i<NT1.size();i++){
		input>>XY[i].x>>XY[i].y;
	}
	input.close();
}

void graph::add_edge(int n1,int n2,double w){
	EDGE.push_back(std::make_tuple(n1,n2,w));
}

void graph::edges_from_table(char* filename){
	ifstream input(filename);
	int n1,n2;
	double w;
	int N1=0,N2=0;
    ind=0;
	while(input>>n1>>n2>>w) N1=max(N1,n1);

	input.clear();
	input.seekg(0, ios::beg);

	while(input>>n1>>n2>>w){
		N2=max(N2,n2);
		add_edge(n1-1,n2-1+N1,w);
	}

	input.close();


    NT1.resize(N1);
	NT2.resize(N2);
	labels1.resize(N1);
	labels2.resize(N2);
	which_cell.resize(N1);
	XY.resize(N1+N2);
	for(unsigned int i=0; i<N1;++i){
		double theta=gsl_rng_uniform(r)*2.0*M_PI;//2.0*M_PI/N1*i;
		XY[i].x = R*cos(theta);
		XY[i].y = R*sin(theta);
	}

	for(unsigned int i=N1; i<N1+N2;++i){
		XY[i].x = gsl_ran_gaussian(r,1);
		XY[i].y = gsl_ran_gaussian(r,1);
		//cout<<XY[i].x<<' ' <<XY[i].y<<endl;
	}

}

void init() {
	glClearColor(0, 0, 0, 1);
	glColor3f(0.0f, 0.0f, 0.0f);
	glPointSize(1.0f);
	glOrtho(-8, 8, -8, 8, 0, 1);
}

void key(unsigned char k, int x, int y){
	ofstream output("graph.dat");
	switch(k){
		case 'r': 
		for(unsigned int i=0;i<G.XY.size(); ++i){
			output<<i<<' '<<G.XY[i].x<<' '<<G.XY[i].y<<endl;
		}
		break;
		case 'i':
		e11=(e11>0) ? 0 : 5e-2;
		break;
        case '+':
        e11*=1.1;
        break;
        case 'k':
        K_circle_bind*=1.1;
        dt*=0.9;
        break;
		case 'q':
		exit(0);
		break;
	}
	output.close();
}

void Timer(int value) {
	glutTimerFunc(50, Timer, 0);
	glutPostRedisplay();
}

void drawBitmapText(string s,float x,float y,void* font, double xshift=0,double yshift=0) 
{  
	char *c;
	char* cstring=(char*)s.c_str();
    glColor3ub(255,255,255);
	glRasterPos2f(x+xshift*zoom, y-yshift*zoom);

	for (c=cstring; *c !='\0'; c++) 
	{
		glutBitmapCharacter(font, *c);
	}
}

void draw_circle(Point& c, double rad, int N=100){
    glBegin(GL_POLYGON);
	for(unsigned int i=0;i<N;i++){
        glColor3ub(253,106,2);
        glVertex2f(c.x+rad*cos(2*M_PI*i/N),c.y+rad*sin(2*M_PI*i/N));
    }
    glEnd();
}

void draw_contour(Point& c, double rad, int N=100){
    glBegin(GL_LINE_LOOP);
	for(unsigned int i=0;i<N;i++){
        glColor3ub(255,255,255);
        glVertex2f(c.x+rad*cos(2*M_PI*i/N),c.y+rad*sin(2*M_PI*i/N));
    }
    glEnd();
}

void load_cell_pos(){
	ifstream input_rad("cell_rad.txt");
	ifstream input_angle("cell_angle.txt");
	for(int i=0;i<7;i++){
		input_rad>>cell_rad[i];
		input_angle>>cell_angle[i];
	}
}

void update(bool verb,int iter, graph& G){
        int i,j,k;
        Point force;
        Point force2;
		Point center;
		Point tmp;
		vector<Point> newXY(G.XY.size());
		double maxforce=0;

        for(k=0;k<iter;k++){
			// TYPE1 nodes
			center.x=0; center.y=0;
			for(i=0;i<G.NT1.size();i++){
				center.x+=G.XY[i].x/G.NT1.size();
				center.y+=G.XY[i].y/G.NT1.size();
			}

            for(i=0;i<G.NT1.size();i++){
				force.x=0; force.y=0;
				
				for(j=0;j<G.NT1.size();j++){
					if(i!=j){
						force.x+=e11*pow(G.XY[i].dist(&G.XY[j]),-3)*(G.XY[i].x-G.XY[j].x);
						force.y+=e11*pow(G.XY[i].dist(&G.XY[j]),-3)*(G.XY[i].y-G.XY[j].y);
					}
				}
				
                
				//force.x+=e1*e0*pow(G.XY[i].dist(&center),-3)*(G.XY[i].x-center.x);
				//force.y+=e1*e0*pow(G.XY[i].dist(&center),-3)*(G.XY[i].y-center.y);

				for(j=G.NT1.size();j<G.XY.size();j++){
					force.x+=e12*pow(G.XY[i].dist(&G.XY[j]),-3)*(G.XY[i].x-G.XY[j].x);
					force.y+=e12*pow(G.XY[i].dist(&G.XY[j]),-3)*(G.XY[i].y-G.XY[j].y);
				}

				for(j=0;j<G.EDGE.size();j++){
					if(get<0>(G.EDGE[j])==i){
						force.x+=-SpringFactor*get<2>(G.EDGE[j])*(G.XY[i].x-G.XY[get<1>(G.EDGE[j])].x);
						force.y+=-SpringFactor*get<2>(G.EDGE[j])*(G.XY[i].y-G.XY[get<1>(G.EDGE[j])].y);
					}
				}

                // To constrain nodes on a ring
				double theta = cell_angle[G.which_cell[i]-1];
				double posx = 10*cos(theta) + cell_rad[G.which_cell[i]-1]*(G.XY[i].x-10*cos(theta))/sqrt(pow(G.XY[i].x-10*cos(theta),2)+pow(G.XY[i].y-10*sin(theta),2));
				double posy = 10*sin(theta) + cell_rad[G.which_cell[i]-1]*(G.XY[i].y-10*sin(theta))/sqrt(pow(G.XY[i].x-10*cos(theta),2)+pow(G.XY[i].y-10*sin(theta),2));
                force.x += -K_circle_bind*(G.XY[i].x-posx);
                force.y += -K_circle_bind*(G.XY[i].y-posy);

				if(force.norm()>maxforce) maxforce=force.norm();

				newXY[i].x=G.XY[i].x+dt*force.x;
				newXY[i].y=G.XY[i].y+dt*force.y;

				// project on disk
				
				// keep fixed
			 	//newXY[i].x=G.XY[i].x;//+dt*force2.x;
				//newXY[i].y=G.XY[i].y;//+dt*force2.y;

			}

			// TYPE2 nodes
			for(i=G.NT1.size();i<G.XY.size();i++){
				force.x=0; force.y=0;
				for(j=0;j<G.NT1.size();j++){
					force.x+=e12*pow(G.XY[i].dist(&G.XY[j]),-3)*(G.XY[i].x-G.XY[j].x);
					force.y+=e12*pow(G.XY[i].dist(&G.XY[j]),-3)*(G.XY[i].y-G.XY[j].y);
				}
				
				for(j=G.NT1.size();j<G.XY.size();j++){
					if(i!=j){
						force.x+=e22*pow(G.XY[i].dist(&G.XY[j]),-3)*(G.XY[i].x-G.XY[j].x);
						force.y+=e22*pow(G.XY[i].dist(&G.XY[j]),-3)*(G.XY[i].y-G.XY[j].y);
					}
				}

				for(j=0;j<G.EDGE.size();j++){
					if(get<1>(G.EDGE[j])==i){
						force.x+=-SpringFactor*get<2>(G.EDGE[j])*(G.XY[i].x-G.XY[get<0>(G.EDGE[j])].x);
						force.y+=-SpringFactor*get<2>(G.EDGE[j])*(G.XY[i].y-G.XY[get<0>(G.EDGE[j])].y);
					}
				}
						
				if(force.norm()>maxforce) maxforce=force.norm();

				newXY[i].x=G.XY[i].x+dt*force.x;
				newXY[i].y=G.XY[i].y+dt*force.y;

			}

			for(i=0;i<G.XY.size();++i){
				G.XY[i].x=newXY[i].x+gsl_ran_gaussian(r,noise);
				G.XY[i].y=newXY[i].y+gsl_ran_gaussian(r,noise);
			}
		}
}

void display(void) {
    int i;
    
	glLoadIdentity();
	glOrtho(delta[0],delta[1], delta[2],delta[3], 0, 1);
	glEnable(GL_LINE_SMOOTH);
	glClear(GL_COLOR_BUFFER_BIT);
    glPointSize(4);
    Point tmp;

    for(i=0;i<G.EDGE.size();i++){
        glColor3ub(75,75,75);
		glLineWidth(floor(get<2>(G.EDGE[i])*1.));
		glBegin(GL_LINES);
        	glVertex2d(G.XY[get<0>(G.EDGE[i])].x, G.XY[get<0>(G.EDGE[i])].y);
        	glVertex2d(G.XY[get<1>(G.EDGE[i])].x, G.XY[get<1>(G.EDGE[i])].y);
    	glEnd();  
    }

    for(i=0;i<G.NT1.size();i++){
		draw_circle(G.XY[i],.05);
	}

	for(i=0; i<7; i++){
		tmp.x=10*cos(cell_angle[i]);
		tmp.y=10*sin(cell_angle[i]);
		draw_contour(tmp,cell_rad[i]);
	}
    
    glBegin(GL_POINTS);
	for(i=G.NT1.size();i<G.XY.size();i++){
        glColor3ub(255,0,255);
        glVertex2f(G.XY[i].x,G.XY[i].y);
    }
    glEnd();
	
	for(i=G.NT1.size();i<G.XY.size();i++){
		drawBitmapText(G.labels2[i-G.NT1.size()],G.XY[i].x,G.XY[i].y,GLUT_BITMAP_HELVETICA_12,0.1);
    }
    
	for(i=0;i<G.NT1.size();i++){
		drawBitmapText(G.labels1[i],G.XY[i].x,G.XY[i].y,GLUT_BITMAP_HELVETICA_12);
	}



    update(0,1,G);
    glutSwapBuffers();
    glFlush();

}

void passive_move(int x,int y){
	//cout<<x<<' '<<y<<"       "<<'\r'<<flush;
}

void on_mouse_move(int x,int y){
	delta[0]=L[0]+(tx0-x)*(L[1]-L[0])/W;
	delta[1]=L[1]+(tx0-x)*(L[1]-L[0])/W;
	delta[2]=L[2]+(y-ty0)*(L[3]-L[2])/H;
	delta[3]=L[3]+(y-ty0)*(L[3]-L[2])/H;
}

void on_mouse(int button,int state,int x,int y){
    int i;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		tx0=x;ty0=y;
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		tx=tx+x-tx0;ty=ty+ty0-y;
	}
	if(button == 4){
		// ZOOM OUT
		zoom=1.1;
		delta[0]=L[0]+(x-zoom*x)*(L[1]-L[0])/W;
		delta[1]=L[0]+(x+zoom*(W-x))*(L[1]-L[0])/W;
		delta[2]=L[3]+(y+(H-y)*zoom)*(L[2]-L[3])/H;
		delta[3]=L[3]+(y-y*zoom)*(L[2]-L[3])/H;
	}
	if(button == 3){
		// ZOOM IN
		zoom=0.9;
		delta[0]=L[0]+(x-zoom*x)*(L[1]-L[0])/W;
		delta[1]=L[0]+(x+zoom*(W-x))*(L[1]-L[0])/W;
		delta[2]=L[3]+(y+(H-y)*zoom)*(L[2]-L[3])/H;
		delta[3]=L[3]+(y-y*zoom)*(L[2]-L[3])/H;
	}
	
	L[0]=delta[0];
	L[1]=delta[1];
	L[2]=delta[2];
	L[3]=delta[3];		
		        
}

int main(int argc, char **argv) {
    int i,j;
	if(argc<6){
		cout<<"usage:"<<endl;
		cout<<"SEED INPUT LABELS PROCESSES init_pos1"<<endl;
		exit(0);
	}
	gsl_rng_set(r,atoi(argv[1]));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(W, H);
	glutCreateWindow("Square");
	glutKeyboardFunc(key);
	glutMouseFunc(on_mouse);
	glutMotionFunc(on_mouse_move);
	glutPassiveMotionFunc(passive_move);
	glutDisplayFunc(display);
	init();
	Timer(0);
    
	G.edges_from_table(argv[2]);
	G.load_labels1(argv[3]); // receptors
	G.load_labels2(argv[4]); // ligands
	G.load_init_pos1(argv[5]); // receptors
    load_cell_pos();

	glutMainLoop();

	return 0;
}
