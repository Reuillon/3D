#version 330 core
out vec4 FragColor;

uniform float isScoped;

float x = 2560,y = 1440;
float alpha = 0.0;

float crosshairSize = 30.0;
float crosshairGap = 50.5;


void main()
{

    if (isScoped <= 0.3) //THIS BLOCK DRAWS DEFAULT CROSSHAIR
    {
        if (((gl_FragCoord.x < (x/2) + crosshairGap +  (crosshairSize * 2) && gl_FragCoord.x > (x/2) + crosshairGap + crosshairSize)  || (gl_FragCoord.x > (x/2) - crosshairGap - (crosshairSize * 2) && gl_FragCoord.x < (x/2) - crosshairGap - (crosshairSize)))  && (gl_FragCoord.y < (y/2) + 1 && gl_FragCoord.y > (y/2) - 1))
        {
            alpha = 1;
            FragColor = vec4(1.0f,1.0f,1.0f, alpha);
            return;
        }
        if (((gl_FragCoord.y < (y/2) + crosshairGap +  (crosshairSize * 2) && gl_FragCoord.y > (y/2) + crosshairGap + crosshairSize)  || (gl_FragCoord.y > (y/2) - crosshairGap - (crosshairSize * 2) && gl_FragCoord.y < (y/2) - crosshairGap - (crosshairSize)))  && (gl_FragCoord.x < (x/2) + 1 && gl_FragCoord.x > (x/2) - 1))
        {
            alpha = 1;
            FragColor = vec4(1.0f,1.0f,1.0f, alpha);
            return;
        }
    }
    else if (isScoped >= 0.8) //THIS BLOCK DRAWS SCOPE OVERLAY
    {
        vec3 color = vec3(0.0);
        vec2 pixelDistance = gl_FragCoord.xy - vec2(x * 0.5, y * 0.5);
        float distance = dot(pixelDistance, pixelDistance);
        float radius = y * y * 0.25 * 0.81;
        
        if (distance > radius)
        {
            alpha = (1 - (radius / distance)) * 0.4;
            clamp(alpha, 0.0,1.0);
            alpha += 0.85;
            color /=2;
            FragColor = vec4(color, alpha);
            return;
        }
        else
        {
          

            if (gl_FragCoord.x > ((x/2) - 1) && gl_FragCoord.x < ((x/2) + 1) || gl_FragCoord.y > ((y/2) - 1) && gl_FragCoord.y < ((y/2) + 1))
            {
                if (distance < x/3)
                {
                    color = vec3(1.0,0.0,0.0);
                }
                alpha = 1.0f;
                FragColor = vec4(color, alpha);
                return;
            }
            if ((gl_FragCoord.y < ((y/2) + (y/150)) && gl_FragCoord.y > ((y/2) - (y/150))) && (int(gl_FragCoord.x) % 80 <= 1) && distance > (x/2))
            {
                alpha = 1.0f;
                FragColor = vec4(color, alpha);
                return;
            }
            if ((gl_FragCoord.x < ((x/2) + (x/300)) && gl_FragCoord.x > ((x/2) - (x/300))) && (int(gl_FragCoord.y) % 80 <= 1) && distance > (x/2))
            {
                alpha = 1.0f;
                FragColor = vec4(color, alpha);
                return;
            }
        }
    }


    
}