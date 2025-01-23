#version 430
layout(local_size_x = 1, local_size_y = 1) in;

layout(rgba32f, binding = 0) uniform image2D outputImg;

void main() {
  // base pixel color
  vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);
  // The Global Work Group ID contains the coordinates of the pixel 
  // that we're rendering 
  ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
  
  //
  // path tracing....
  //

  pixel.x = float(pixelCoords.x)/(gl_NumWorkGroups.x);
  pixel.y = float(pixelCoords.y)/(gl_NumWorkGroups.y);
  
  // store output pixel color
  imageStore(outputImg, pixelCoords, pixel);
}