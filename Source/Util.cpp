#include "Util.h"

void json_setting(const nlohmann::json& j, JSON_SETTING& h)
{
	j.at("input_file_path").get_to(h.input_file_path);
	j.at("out_file_path").get_to(h.out_file_path);
	j.at("enable_obj_compress").get_to(h.enable_obj_compress);
	j.at("is_point_cloud").get_to(h.is_point_cloud);
	j.at("pos_quantization_bits").get_to(h.pos_quantization_bits);
	j.at("tex_coords_quantization_bits").get_to(h.tex_coords_quantization_bits);
	j.at("tex_coords_deleted").get_to(h.tex_coords_deleted);
	j.at("normals_quantization_bits").get_to(h.normals_quantization_bits);
	j.at("normals_deleted").get_to(h.normals_deleted);
	j.at("generic_quantization_bits").get_to(h.generic_quantization_bits);
	j.at("generic_deleted").get_to(h.generic_deleted);
	j.at("compression_level").get_to(h.compression_level);
	j.at("use_metadata").get_to(h.use_metadata);
	j.at("enable_png_compress").get_to(h.enable_png_compress);
}

void ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;

	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}