uniform vec4 color;

varying vec3 n;
varying vec3 l;

const float ka = 0.2;
const float kd = 1.0;


void main(void)
{
    vec4 diffColor = color;

    vec3 n2 = normalize(n);
    vec3 l2 = normalize(l);

    gl_FragColor = diffColor * (ka + kd * max(dot(n2, l2), 0.0f));
//    gl_FragColor = vec4(n2, 1.0f);
//    vec4(0.0f, 1.0f, 0.0f, 1.0f);
}
