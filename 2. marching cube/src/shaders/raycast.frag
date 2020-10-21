#version 330 core

bool intersectRayAABB(vec3 ray_o, vec3 ray_d, vec3 min_box, vec3 max_box, inout float t0, inout float t1)
{
	vec3 dirfrac = 1.0 / ray_d;

	vec3 a = (min_box - ray_o) * dirfrac;
	vec3 b = (max_box - ray_o) * dirfrac;

	vec3 c = min(a, b);
	vec3 d = max(a, b);

	float tmin = max(max(c.x, c.y), c.z);
	float tmax = min(min(d.x, d.y), d.z);

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	// if tmin > tmax, ray doesn't intersect AABB
	if (tmax < 0.0 || tmin > tmax)
	    return false;

	t0 = max(tmin, 0.0);
	t1 = tmax;
	return true;
}

vec3 interpolateViewDir(vec3 view_dir[4], vec2 tex_coord)
{
	vec3 btm_d = mix(view_dir[0], view_dir[2], tex_coord.x);
	vec3 top_d = mix(view_dir[1], view_dir[3], tex_coord.x);
	return normalize(mix(btm_d, top_d, tex_coord.y));
}

vec3 worldToVolumeCoord(vec3 x, vec3 min_box, vec3 max_box)
{
	return (x - min_box) / (max_box - min_box);
}

vec3 forwardDiffGradient(sampler3D tex_vol, vec3 x, float dt)
{
	// TODO: Implement me!
	return vec3(0);
}

vec3 centralDiffGradient(sampler3D tex_vol, vec3 x, float dt)
{
	// TODO: Or me!
	return vec3(0);
}

uniform sampler3D tex_vol;
uniform sampler1D tex_tf;
uniform vec3 view_pos;
uniform vec3 view_dir[4];
uniform vec3 size = vec3(1);
uniform float stepsize;
uniform float lod = 0.0;

in vec2 tex_coord;
layout(location = 0) out vec4 out_color;

void main()
{
	// Calc extends of volume
	vec3 min_box = vec3(-0.5) * size;
	vec3 max_box = vec3( 0.5) * size;

	// Setup ray param
	vec3 ray_d = interpolateViewDir(view_dir, tex_coord);
	vec3 ray_o = view_pos;

	// Reset Color and Alpha
	vec3 C = vec3(0);
	float A = 0;

	// Test intersection between ray and bounding box
	float t0, t1;
	if (intersectRayAABB(ray_o, ray_d, min_box, max_box, t0, t1)) {

		// Initialize t to entry point
		float t = t0;

		while (t < t1 && A < 1.0) {
			// World Space Position
			vec3 p = ray_o + ray_d * t;

			// Volume Space Coordinates
			vec3 coord = worldToVolumeCoord(p, min_box, max_box);

			// Intensity value of volume data
			float s = textureLod(tex_vol, coord, lod).r;

			// Transfer function lookup
			vec4 color = texture(tex_tf, s);
			color.a *= stepsize * 100;

			// TODO: forward-to-back emission absorption
			// Hint: the lines above examples on how to
			// sample the volume and the transferfunction.
			C += (1 - A) * color.a * color.rgb;
			A += (1 - A) * color.a;

			t += stepsize;
		}
	}

	// Write Color to pixel
   	out_color = vec4(C, 1);
}