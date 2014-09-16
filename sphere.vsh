attribute vec4 vertexis;
uniform mat4 modelViewProj;

void main(void)
{
    vec4 trueVertex;
    trueVertex.xyz = vertexis.xyz;
    trueVertex.w = 1.0f;
    gl_Position = modelViewProj*trueVertex;
}
