#ifndef ENGINE_EASYOCR_HPP
#define ENGINE_EASYOCR_HPP

#include <Python.h>
#include <vector>
#include "engine.hpp"

namespace ocr
{

    class engine_easyocr : public engine
    {

    public:
        engine_easyocr();
        ~engine_easyocr();

        void init(const std::string &path) override;
        std::string text() override;
        int conf() override;
        void overlay(const std::vector<std::vector<int>> &bounding_boxes) override;

        void print();

    private:
    };

} // namespace ocr

#endif // ENGINE_EASYOCR_HPP