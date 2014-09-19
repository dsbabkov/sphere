attribute vec4 vertexis;

uniform mat4 modelViewProj;
uniform mat3 normalMatr;
uniform mat4 modelView;
uniform int normMethod;

varying vec3 n;
varying vec3 l;



const vec3 lightPos = vec3(0, 1, -1);


vec3 normSphere(vec3 vertex)
{
    return normalize(vertex);
}

vec3 normCylinder(vec3 vertex)
{
    return normalize(vec3(vertex.x, 0.0f, vertex.z));
}

vec3 normCone(vec3 vertex)
{
//    float radius = length(vertex.xz);
//    float y = radius * 3.0f;
//    return normalize(vec3(vertex.x, y, vertex.z));
//    return vec3(-1.0f, 0.0f, 0.0f);
    return -normCylinder(vertex);

}

vec3 normDisk()
{
    return vec3(0.0f, -1.0f, 0.0f);
}

vec3 getNormal(vec3 vertex)
{
    if (normMethod == 0)
        return normSphere(vertex);
    else if (normMethod == 1)
        return normCone(vertex);
    else if (normMethod == 2)
        return normCylinder(vertex);
    else if (normMethod == 3)
        return normDisk();
}

void main(void)
{
    gl_Position = modelViewProj * vec4(vertexis);

    vec3 p = vec3(modelView * vertexis);



    l = normalize(lightPos - p);
    n = normalize(normalMatr * getNormal(vertexis.xyz));
}

