/* Playing with GLFW */
#define	GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#define WIDTH	640
#define HEIGHT	640

char buf[BUFSIZ];
int ret;

void glfw_err(int no,const char *msg)
{
	fprintf(stderr,"glfw error %d: %s\n",no,msg);
	exit(-2);
}

void glfw_key(GLFWwindow *win,int key,int scan,int act,int mods)
{
	if(key==GLFW_KEY_Q && act==GLFW_PRESS)
		glfwSetWindowShouldClose(win,GLFW_TRUE);
}

void load_shader(int id,const char *srcpath)
{
	FILE *src;
	char *srcptr=buf;
	printf("Loading shader `%s'\n",srcpath);
	if(!(src=fopen(srcpath,"rb"))){
		perror("fopen() failed");
		exit(-1);
	}
	fread(buf,BUFSIZ,1,src);
	fclose(src);
	glShaderSource(id,1,&srcptr,NULL);
	memset(buf,0,BUFSIZ);
}

void compile_shader(int id)
{
	int len;
	puts("Compiling ...");
	glCompileShader(id);
	glGetShaderiv(id,GL_COMPILE_STATUS,&ret);
	glGetShaderiv(id,GL_INFO_LOG_LENGTH,&len);
	if(len>0){
		glGetShaderInfoLog(id,len,NULL,buf);
		printf("%s\n",buf);
	}
}

int load_shaders(const char *vshader,const char *fshader)
{
	int v_id=glCreateShader(GL_VERTEX_SHADER);
	int f_id=glCreateShader(GL_FRAGMENT_SHADER);
	int prgm_id=glCreateProgram();
	int len;
	load_shader(v_id,vshader);
	compile_shader(v_id);
	load_shader(f_id,fshader);
	compile_shader(f_id);
	puts("Linking shaders");
	glAttachShader(prgm_id,v_id);
	glAttachShader(prgm_id,f_id);
	glLinkProgram(prgm_id);
	glGetProgramiv(prgm_id,GL_LINK_STATUS,&ret);
	glGetProgramiv(prgm_id,GL_INFO_LOG_LENGTH,&len);
	if(len>0){
		glGetProgramInfoLog(prgm_id,len,NULL,buf);
		printf("%s\n",buf);
	}
	glDetachShader(prgm_id,v_id);
	glDetachShader(prgm_id,f_id);
	glDeleteShader(v_id);
	glDeleteShader(f_id);
	return prgm_id;
}

float vert_buffer[]={
	-1.0f,-1.0f,0.0f,
	1.0f,-1.0f,0.0f,
	1.0f,1.0f,0.0f,
	-1.0f,1.0f,0.0f,
	-1.0f,-1.0f,0.0f,
	1.0f,1.0f,0.0f
};
uint32_t vertarr_id;
uint32_t vertbuf_id;
float prev_x=0.0f,prev_y=0.0f;
const float step=0.0001f;

void get_pt(GLFWwindow *win,double *x,double *y)
{
	float z;
	glfwGetCursorPos(win,x,y);
	if(*x<=(double)WIDTH && *y<=(double)HEIGHT
	&& *x>=0 && *y>=0){
		*x=*x/WIDTH*2-1;
		*y=1-*y/HEIGHT*2;
		float dx=*x-prev_x;
		float dy=*y-prev_y;
		if(dx!=0.0f || dy!=0.0f){
			z=sqrt(dx*dx+dy*dy);
			prev_x+=dx/z*0.001f;
			prev_y+=dy/z*0.001f;
			*x=prev_x;
			*y=prev_y;
		}
		return;
	}
	if(prev_x!=0.0f || prev_y!=0.0f){
		float k=40.0f;
		z=sqrt(prev_x*prev_x+prev_y*prev_y);
		prev_x+=(k*prev_y-prev_x/z)*step;
		prev_y+=(-prev_y/z-k*prev_x)*step;
	}
	*x=prev_x;
	*y=prev_y;
}

void render(GLFWwindow *win,int prgm)
{
	double x,y;
	double time=glfwGetTime();
	get_pt(win,&x,&y);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glUseProgram(prgm);
	int time_id=glGetUniformLocation(prgm,"time");
	glUniform1f(time_id,time);
	int pt_id=glGetUniformLocation(prgm,"pt");
	glUniform2f(pt_id,x,y);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,vertbuf_id);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
	glDrawArrays(GL_TRIANGLES,0,6);
	glDisableVertexAttribArray(0);
}

int main(void)
{
	GLFWwindow *win;
	ret=glfwInit();
	glfwSetErrorCallback(&glfw_err);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,2);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	win=glfwCreateWindow(WIDTH,HEIGHT,"Hello there",NULL,NULL);
	glfwSetKeyCallback(win,&glfw_key);
	assert(win);

	glfwMakeContextCurrent(win);

	glGenVertexArrays(1,&vertarr_id);	// Generate one VAO
	glBindVertexArray(vertarr_id);
	glGenBuffers(1,&vertbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER,vertbuf_id);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vert_buffer),vert_buffer,
			GL_STATIC_DRAW);
	int prgm=load_shaders("vertex.vert","fragment.frag");

	while(!glfwWindowShouldClose(win)){
		int w,h;
		glfwGetFramebufferSize(win,&w,&h);
		glViewport(0,0,w,h);
		render(win,prgm);
		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	glfwDestroyWindow(win);
	glfwTerminate();
	return 0;
}
