/* @binding(0) @group(0) var<uniform> frame : u32; */

@vertex
fn vs_main(@builtin(vertex_index) vertex_index: u32) -> @builtin(position) vec4f {
    const pos = array(
        vec2( 0.0,  0.5),
        vec2(-0.5, -0.5),
        vec2( 0.5, -0.5),
    );

    return vec4f(pos[vertex_index], 0, 1);
}

@fragment
fn fs_main() -> @location(0) vec4f {
    return vec4f(1.0, 0.0, 0.0, 1.0);
    /* return vec4f(1, sin(f32(frame) / 128), 0, 1); */
}
