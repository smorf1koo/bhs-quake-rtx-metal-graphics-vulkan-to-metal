// obj_loader.h — minimal Wavefront OBJ loader for test task 01.
//
// Helper code, not part of the task: it saves you from writing a parser.
// Supports exactly what assets/cube.obj uses: v / vt / vn / f lines (polygons
// are fan-triangulated). Materials, groups, negative indices etc. are ignored.
//
// You may replace it with your own loader or tinyobjloader — say so in README.
#pragma once

#include <array>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace bhs {

struct ObjVertex {
    float position[3];
    float uv[2];      // as in the file: v points UP. Flipping v is up to you.
    float normal[3];
};

struct ObjMesh {
    std::vector<ObjVertex> vertices;  // unique v/vt/vn combinations
    std::vector<uint32_t> indices;    // triangles, CCW when seen from outside
};

// Returns false and prints the reason to stderr if the file cannot be read.
inline bool LoadObj(const std::string& path, ObjMesh& out) {
    std::ifstream in(path);
    if (!in) {
        std::fprintf(stderr, "LoadObj: cannot open %s\n", path.c_str());
        return false;
    }
    std::vector<std::array<float, 3>> pos, nrm;
    std::vector<std::array<float, 2>> tex;
    std::unordered_map<std::string, uint32_t> cache;
    out = {};

    auto vertexFor = [&](const std::string& token) -> int64_t {
        auto it = cache.find(token);
        if (it != cache.end()) return it->second;
        int vi = 0, ti = 0, ni = 0;
        // Formats: v, v/vt, v//vn, v/vt/vn
        if (std::sscanf(token.c_str(), "%d/%d/%d", &vi, &ti, &ni) != 3 &&
            std::sscanf(token.c_str(), "%d//%d", &vi, &ni) != 2 &&
            std::sscanf(token.c_str(), "%d/%d", &vi, &ti) != 2 &&
            std::sscanf(token.c_str(), "%d", &vi) != 1)
            return -1;
        if (vi < 1 || vi > (int)pos.size() || ti > (int)tex.size() || ni > (int)nrm.size()) return -1;
        ObjVertex v{};
        for (int k = 0; k < 3; ++k) v.position[k] = pos[vi - 1][k];
        if (ti > 0) { v.uv[0] = tex[ti - 1][0]; v.uv[1] = tex[ti - 1][1]; }
        if (ni > 0) for (int k = 0; k < 3; ++k) v.normal[k] = nrm[ni - 1][k];
        uint32_t idx = (uint32_t)out.vertices.size();
        out.vertices.push_back(v);
        cache.emplace(token, idx);
        return idx;
    };

    std::string line;
    int lineNo = 0;
    while (std::getline(in, line)) {
        ++lineNo;
        std::istringstream ss(line);
        std::string tag;
        ss >> tag;
        if (tag == "v") {
            std::array<float, 3> p{}; ss >> p[0] >> p[1] >> p[2]; pos.push_back(p);
        } else if (tag == "vt") {
            std::array<float, 2> t{}; ss >> t[0] >> t[1]; tex.push_back(t);
        } else if (tag == "vn") {
            std::array<float, 3> n{}; ss >> n[0] >> n[1] >> n[2]; nrm.push_back(n);
        } else if (tag == "f") {
            std::vector<uint32_t> poly;
            std::string tok;
            while (ss >> tok) {
                int64_t idx = vertexFor(tok);
                if (idx < 0) {
                    std::fprintf(stderr, "LoadObj: bad face index '%s' at line %d\n", tok.c_str(), lineNo);
                    return false;
                }
                poly.push_back((uint32_t)idx);
            }
            for (size_t i = 2; i < poly.size(); ++i) {
                out.indices.push_back(poly[0]);
                out.indices.push_back(poly[i - 1]);
                out.indices.push_back(poly[i]);
            }
        }
    }
    return !out.indices.empty();
}

}  // namespace bhs
