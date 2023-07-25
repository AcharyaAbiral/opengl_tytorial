#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform float max_y;
in float pos_y;

void main()
{
	//if(y_pos>4.5)
	//{
		//FragColor=texture(texture3,TexCoord);
	//}
	//else
	//{
		//FragColor=texture(texture3,TexCoord);
		//*vec4(0.0,y_pos/4,0.0,1.0);
	//}
   //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), y_pos/5);

   if(pos_y>=0.96*max_y)
   {
	   FragColor=texture(texture3,TexCoord);
   }
   else if (pos_y>=0.9*max_y)
   {
	   float m=(pos_y-0.9*max_y)/(0.06*max_y);
	   FragColor=mix(texture(texture1,TexCoord),texture(texture3,TexCoord),m);
   }
   else if(pos_y>0.86*max_y)
   {
	   FragColor=texture(texture1,TexCoord);
   }
   else if(pos_y>=0.79*max_y)
   {
	   float m=(pos_y-0.79*max_y)/(0.07*max_y);
	   FragColor=mix(texture(texture2,TexCoord),texture(texture1,TexCoord),m);
   }
   else
   {
	    FragColor=texture(texture2,TexCoord);
   }


}