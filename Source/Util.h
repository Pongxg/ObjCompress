#pragma once
#include <string>
#include "nlohmann/json.hpp"
#include "easylogging++.h"


typedef std::map<std::string, std::string> StrMap;
typedef StrMap::iterator StrMapIter;
typedef std::pair<StrMapIter, bool > StrMapPair;

typedef struct _JSON_SETTING {
	std::string input_file_path = "";
    std::string out_file_path = "";
    bool enable_obj_compress = true;
    bool is_point_cloud = false;
    int pos_quantization_bits = 11;
    int tex_coords_quantization_bits = 10;
    bool tex_coords_deleted = false;
    int normals_quantization_bits = 8;
    bool normals_deleted = false;
    int generic_quantization_bits = 8;
    bool generic_deleted = false;
    int compression_level = 7;
    bool use_metadata = false;
    bool enable_png_compress = false;
}JSON_SETTING, *PJSON_SETTING;

void json_setting(const nlohmann::json& j, JSON_SETTING& h);
void ReplaceAll(std::string& str, const std::string& from, const std::string& to);