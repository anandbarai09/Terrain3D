#ifndef OCEAN3D_CLASS_H
#define OCEAN3D_CLASS_H

#ifndef __FLT_MAX__
#define __FLT_MAX__ FLT_MAX
#endif

#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/geometry_instance3d.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>

#include "ocean_3d_material.h"
#include "ocean_3d_storage.h"

using namespace godot;

class Ocean3D : public Node3D {
	GDCLASS(Ocean3D, Node3D);

public:
	// Constants
	static inline const char *__class__ = "Ocean3D";

private:
	// Ocean state
	String _version = "0.9.2-dev";
	bool _is_inside_world = false;
	bool _initialized = false;

	// Ocean settings
	int _mesh_size = 48;
	int _mesh_lods = 7;
	real_t _mesh_vertex_spacing = 1.0f;

	Ref<Ocean3DStorage> _storage;
	Ref<Ocean3DMaterial> _material;

	// Editor components
	EditorPlugin *_plugin = nullptr;
	// Current editor or gameplay camera we are centering the terrain on.
	Camera3D *_camera = nullptr;
	// X,Z Position of the camera during the previous snapping. Set to max real_t value to force a snap update.
	Vector2 _camera_last_position = Vector2(__FLT_MAX__, __FLT_MAX__);

	Vector3 CenterPosition = Vector3(0, 0, 0);

	// Meshes and Mesh instances
	Vector<RID> _meshes;
	struct Instances {
		RID cross;
		Vector<RID> tiles;
		Vector<RID> fillers;
		Vector<RID> trims;
		Vector<RID> seams;
		Vector<RID> skirts;
	} _data;

	// Renderer settings
	uint32_t _render_layers = 1 | (1 << 31); // Bit 1 and 32 for the cursor
	GeometryInstance3D::ShadowCastingSetting _shadow_casting = GeometryInstance3D::SHADOW_CASTING_SETTING_OFF;
	real_t _cull_margin = 0.0f;

	// Mouse cursor
	SubViewport *_mouse_vp = nullptr;
	Camera3D *_mouse_cam = nullptr;
	MeshInstance3D *_mouse_quad = nullptr;
	uint32_t _mouse_layer = 32;

	void _initialize();
	void __ready();
	void __process(double delta);

	void _setup_mouse_picking();
	void _destroy_mouse_picking();
	void _grab_camera();
	void _find_cameras(TypedArray<Node> from_nodes, Node *excluded_node, TypedArray<Camera3D> &cam_array);

	void _clear(bool p_clear_meshes = true);
	void _build(int p_mesh_lods, int p_mesh_size);

	void _update_instances();

	void _generate_triangles(PackedVector3Array &p_vertices, PackedVector2Array *p_uvs, int32_t p_lod, Ocean3DStorage::HeightFilter p_filter, bool require_nav, AABB const &p_global_aabb) const;
	void _generate_triangle_pair(PackedVector3Array &p_vertices, PackedVector2Array *p_uvs, int32_t p_lod, Ocean3DStorage::HeightFilter p_filter, bool require_nav, int32_t x, int32_t z) const;

public:
	static int debug_level;
	int sea_level = 10;

	Ocean3D();
	~Ocean3D();

	// Terrain settings
	String get_version() const { return _version; }
	void set_debug_level(int p_level);
	int get_debug_level() const { return debug_level; }
	void set_mesh_lods(int p_count);
	int get_mesh_lods() const { return _mesh_lods; }
	void set_mesh_size(int p_size);
	int get_mesh_size() const { return _mesh_size; }
	void set_mesh_vertex_spacing(real_t p_spacing);
	real_t get_mesh_vertex_spacing() const { return _mesh_vertex_spacing; }

	void set_storage(const Ref<Ocean3DStorage> &p_storage);
	Ref<Ocean3DStorage> get_storage() const { return _storage; }
	void set_material(const Ref<Ocean3DMaterial> &p_material);
	Ref<Ocean3DMaterial> get_material() const { return _material; }

	// Editor components
	void set_plugin(EditorPlugin *p_plugin);
	EditorPlugin *get_plugin() const { return _plugin; }
	void set_camera(Camera3D *p_plugin);
	Camera3D *get_camera() const { return _camera; }

	// Renderer settings
	void set_render_layers(uint32_t p_layers);
	uint32_t get_render_layers() const { return _render_layers; };
	void set_mouse_layer(uint32_t p_layer);
	uint32_t get_mouse_layer() const { return _mouse_layer; };
	void set_cast_shadows(GeometryInstance3D::ShadowCastingSetting p_shadow_casting);
	GeometryInstance3D::ShadowCastingSetting get_cast_shadows() const { return _shadow_casting; };
	void set_cull_margin(real_t p_margin);
	real_t get_cull_margin() const { return _cull_margin; };

	// Terrain methods
	void snap(Vector3 p_cam_pos);
	void update_aabbs();
	Vector3 get_intersection(Vector3 p_src_pos, Vector3 p_direction);

	PackedStringArray _get_configuration_warnings() const override;

protected:
	void _notification(int p_what);
	static void _bind_methods();
};

#endif // OCEAN3D_CLASS_H