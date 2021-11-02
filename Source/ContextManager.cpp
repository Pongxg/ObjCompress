#include  "ContextManager.h"
#include  <iostream>
#include  <nlohmann/json.hpp>
#include  <fstream>
#include  "Util.h"
#include <filesystem>
//png
#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include "../imagequant/libimagequant.h"

#include "draco/io/stdio_file_reader.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#endif
#include "draco/io/file_reader_factory.h"



using json = nlohmann::json;
ContextManager* Singleton<ContextManager>::m_pSingleton = NULL;
ContextManager::ContextManager():
    m_strConfigName("")
{

}

ContextManager::~ContextManager() {

}


bool ContextManager::ParserCmdLine(int argc, char** argv)
{
    int config = 0;
    std::string arg = argv[1];
    if (arg == "-c")
    {
        if (2 < argc) {
            m_strConfigName = argv[2];
            return true;
        }
    }
    return  false;
}

bool ContextManager::ParserConfig()
{
    nlohmann::json json_dto;
    std::ifstream jfile(m_strConfigName.c_str());
    if (!jfile.is_open())
    {
        std::cout << "open test.json failed" << std::endl;
        return false;
    }
    try
    {
        jfile >> json_dto;
        if (json_dto.is_discarded())
        {
            std::cout << "parse json data failed" << std::endl;
            return false;
        }
        json_setting(json_dto, m_tJosnSetting);
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return false;
    }
    return  true;
}

void ContextManager::TraversePath()
{
    const std::filesystem::path rootPath(m_tJosnSetting.input_file_path);
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ rootPath })
        if (dir_entry.path().filename() == "model.obj" || dir_entry.path().filename() == "model.png") {
            std::string filePath = dir_entry.path().string();
            ReplaceAll(filePath, "\\", "/");
            std::string fileName = dir_entry.path().filename().replace_extension().string();
            std::string parentPath = dir_entry.path().parent_path().string();
            std::string newPath = parentPath;
            ReplaceAll(newPath, "\\", "/");
            ReplaceAll(newPath, m_tJosnSetting.input_file_path, m_tJosnSetting.out_file_path);
            m_mapFile.insert(std::make_pair(parentPath, newPath));
            if (dir_entry.path().extension() == ".obj")
            {
                std::string newFileName = newPath + "/" + fileName + ".bobj";
                StrMapPair pairIter = m_mapFileObj.insert(std::make_pair(filePath, newFileName));
                if (!pairIter.second)
                {
                    LOG(ERROR) << "FileManager:: obj file already exists:" << fileName << " path:" << filePath;
                }

            }
            else
            {
                std::string newFileName = newPath + "/" + dir_entry.path().filename().string();
                StrMapPair pairIter = m_mapFilePng.insert(std::make_pair(filePath, newFileName));
                if (!pairIter.second)
                {
                    LOG(ERROR) << "FileManager:: png file already exists:" << fileName << " path:" << filePath;
                }
            }
        }
}

bool ContextManager::MakePathDir()
{
    
    for (StrMapIter iter = m_mapFile.begin(); iter != m_mapFile.end(); ++iter)
    {
        bool  ret= std::filesystem::create_directories(iter->second);
        if (!ret)
        {
            LOG(WARNING) << "MakePathDir:: create path fail:" << " path:" << iter->second;
        }

    }
    return true;
}

bool ContextManager::EncodeFiles()
{
    if (m_tJosnSetting.enable_obj_compress)
    {
        for (StrMapIter iter = m_mapFileObj.begin(); iter != m_mapFileObj.end(); ++iter)
        {
            EncodeObj(iter->first, iter->second);
        }
    }

    if (m_tJosnSetting.enable_png_compress)
    {

        for (StrMapIter iter = m_mapFilePng.begin(); iter != m_mapFilePng.end(); ++iter)
        {
            EncodePng(iter->first, iter->second);
        }
    }
    return true;
}

bool ContextManager::EncodeObj(std::string filePath, std::string newPath)
{
    std::unique_ptr<draco::PointCloud> pc;
    draco::Mesh* mesh = nullptr;

    auto maybe_mesh =draco::ReadMeshFromFile(filePath, m_tJosnSetting.use_metadata);
    if (!maybe_mesh.ok()) {
        LOG(ERROR)<<"Failed loading the input mesh:"<< filePath <<"\n" << maybe_mesh.status().error_msg();
        return -1;
    }
    mesh = maybe_mesh.value().get();
    pc = std::move(maybe_mesh).value();


    if (m_tJosnSetting.pos_quantization_bits < 0) {
        LOG(ERROR) << "Error: Position attribute cannot be skipped.";
        return -1;
    }

    // Delete attributes if needed. This needs to happen before we set any
    // quantization settings.
    if (m_tJosnSetting.tex_coords_quantization_bits < 0) {
        if (pc->NumNamedAttributes(draco::GeometryAttribute::TEX_COORD) > 0) {
            m_tJosnSetting.tex_coords_deleted = true;
        }
        while (pc->NumNamedAttributes(draco::GeometryAttribute::TEX_COORD) > 0) {
            pc->DeleteAttribute(
                pc->GetNamedAttributeId(draco::GeometryAttribute::TEX_COORD, 0));
        }
    }
    if (m_tJosnSetting.normals_quantization_bits < 0) {
        if (pc->NumNamedAttributes(draco::GeometryAttribute::NORMAL) > 0) {
            m_tJosnSetting.normals_deleted = true;
        }
        while (pc->NumNamedAttributes(draco::GeometryAttribute::NORMAL) > 0) {
            pc->DeleteAttribute(
                pc->GetNamedAttributeId(draco::GeometryAttribute::NORMAL, 0));
        }
    }
    if (m_tJosnSetting.generic_quantization_bits < 0) {
        if (pc->NumNamedAttributes(draco::GeometryAttribute::GENERIC) > 0) {
            m_tJosnSetting.generic_deleted = true;
        }
        while (pc->NumNamedAttributes(draco::GeometryAttribute::GENERIC) > 0) {
            pc->DeleteAttribute(
                pc->GetNamedAttributeId(draco::GeometryAttribute::GENERIC, 0));
        }
    }
#ifdef DRACO_ATTRIBUTE_INDICES_DEDUPLICATION_SUPPORTED
    // If any attribute has been deleted, run deduplication of point indices again
    // as some points can be possibly combined.
    if (m_tJosnSetting.tex_coords_deleted || m_tJosnSetting.normals_deleted ||
        m_tJosnSetting.generic_deleted) {
        pc->DeduplicatePointIds();
    }
#endif

    // Convert compression level to speed (that 0 = slowest, 10 = fastest).
    const int speed = 10 - m_tJosnSetting.compression_level;

    draco::Encoder encoder;

    // Setup encoder options.
    if (m_tJosnSetting.pos_quantization_bits > 0) {
        encoder.SetAttributeQuantization(draco::GeometryAttribute::POSITION,
            m_tJosnSetting.pos_quantization_bits);
    }
    if (m_tJosnSetting.tex_coords_quantization_bits > 0) {
        encoder.SetAttributeQuantization(draco::GeometryAttribute::TEX_COORD,
            m_tJosnSetting.tex_coords_quantization_bits);
    }
    if (m_tJosnSetting.normals_quantization_bits > 0) {
        encoder.SetAttributeQuantization(draco::GeometryAttribute::NORMAL,
            m_tJosnSetting.normals_quantization_bits);
    }
    if (m_tJosnSetting.generic_quantization_bits > 0) {
        encoder.SetAttributeQuantization(draco::GeometryAttribute::GENERIC,
            m_tJosnSetting.generic_quantization_bits);
    }
    encoder.SetSpeedOptions(speed, speed);

    if (newPath.empty()) {
        // Create a default output file by attaching .drc to the input file name.
        newPath = filePath + ".drc";
    }

    int ret = -1;
    ret = EncodeMeshToFile(*mesh, newPath, &encoder);
    if (ret != -1 && m_tJosnSetting.compression_level < 10) {
        LOG(INFO) << "For better compression, increase the compression level up to '-cl 10' ";
    }

    return ret;
}

bool ContextManager::EncodePng(std::string filePath, std::string newPath)
{
    const char* input_png_file_path = filePath.c_str();

    // Load PNG file and decode it as raw RGBA pixels
    // This uses lodepng library for PNG reading (not part of libimagequant)

    unsigned int width, height;
    unsigned char* raw_rgba_pixels;
    unsigned int status = lodepng_decode32_file(&raw_rgba_pixels, &width, &height, input_png_file_path);
    if (status) {
        LOG(ERROR) << "Can't load :"<<input_png_file_path <<"\n"<<lodepng_error_text(status);
        return EXIT_FAILURE;
    }

    // Use libimagequant to make a palette for the RGBA pixels

    liq_attr* handle = liq_attr_create();
    liq_image* input_image = liq_image_create_rgba(handle, raw_rgba_pixels, width, height, 0);
    // You could set more options here, like liq_set_quality
    liq_result* quantization_result;
    if (liq_image_quantize(input_image, handle, &quantization_result) != LIQ_OK) {
        LOG(ERROR) << "Quantization failed:" << filePath;
        return EXIT_FAILURE;
    }

    // Use libimagequant to make new image pixels from the palette

    size_t pixels_size = width * height;
    unsigned char* raw_8bit_pixels = (unsigned char*)malloc(pixels_size);
    liq_set_dithering_level(quantization_result, 1.0);

    liq_write_remapped_image(quantization_result, input_image, raw_8bit_pixels, pixels_size);
    const liq_palette* palette = liq_get_palette(quantization_result);

    // Save converted pixels as a PNG file
    // This uses lodepng library for PNG writing (not part of libimagequant)

    LodePNGState state;
    lodepng_state_init(&state);
    state.info_raw.colortype = LCT_PALETTE;
    state.info_raw.bitdepth = 8;
    state.info_png.color.colortype = LCT_PALETTE;
    state.info_png.color.bitdepth = 8;

    for (int i = 0; i < palette->count; i++) {
        lodepng_palette_add(&state.info_png.color, palette->entries[i].r, palette->entries[i].g, palette->entries[i].b, palette->entries[i].a);
        lodepng_palette_add(&state.info_raw, palette->entries[i].r, palette->entries[i].g, palette->entries[i].b, palette->entries[i].a);
    }

    unsigned char* output_file_data;
    size_t output_file_size;
    unsigned int out_status = lodepng_encode(&output_file_data, &output_file_size, raw_8bit_pixels, width, height, &state);
    if (out_status) {
        LOG(ERROR) << "Can't encode image: %s:" << filePath << "\n" << lodepng_error_text(out_status);
        return EXIT_FAILURE;
    }

    const char* output_png_file_path = newPath.c_str();
    FILE* fp = fopen(output_png_file_path, "wb");
    if (!fp) {
        LOG(ERROR) << "Can't encode image: %s:" << filePath << " out:" << output_png_file_path;
        return EXIT_FAILURE;
    }
    fwrite(output_file_data, 1, output_file_size, fp);
    fclose(fp);

    LOG(INFO) << "Written %s:" << filePath << " out:" << output_png_file_path;

    // Done. Free memory.

    liq_result_destroy(quantization_result); // Must be freed only after you're done using the palette
    liq_image_destroy(input_image);
    liq_attr_destroy(handle);

    free(raw_8bit_pixels);
    lodepng_state_cleanup(&state);

    return true;
}

int ContextManager::EncodeMeshToFile(const draco::Mesh& mesh, const std::string& file,
    draco::Encoder* encoder) {
    draco::CycleTimer timer;
    // Encode the geometry.
    draco::EncoderBuffer buffer;
    timer.Start();
    const draco::Status status = encoder->EncodeMeshToBuffer(mesh, &buffer);
    if (!status.ok()) {
        LOG(ERROR) << "Failed to encode the mesh:"<<file<<"\n"<< status.error_msg();
        return -1;
    }
    timer.Stop();
    // Save the encoded geometry into a file.
    if (!draco::WriteBufferToFile(buffer.data(), buffer.size(), file)) {
        LOG(ERROR) << "Failed to create the output file:" << file;
        return -1;
    }
    LOG(INFO) << "Encoded mesh saved to"<< file <<" time:"<< timer.GetInMs()<<"ms to encode";
    LOG(INFO) << "Encoded size:"<<buffer.size()<<"bytes";
    return 0;
}