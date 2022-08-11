#include "engine_tesseract.hpp"

namespace ocr
{

    engine_tesseract::engine_tesseract(const std::string &path)
    {
        set_engine(name::tesseract);
        set_path(path);
        m_api = new tesseract::TessBaseAPI();
    }

    engine_tesseract::~engine_tesseract()
    {
        for (auto box : bounding_box_storage)
        {
            boxDestroy(&box);
        }
        pixDestroy(&m_img_pix);
        m_img_cv.release();
        m_api->End();
        delete m_api;
    }

    void engine_tesseract::init()
    {
        int orient_deg;
        float orient_conf;
        if (m_api->Init(nullptr, "deu", tesseract::OEM_LSTM_ONLY))
        {
            std::cerr << "Error: Could not initialize Tesseract" << std::endl;
        }
        else
        {
            m_img_pix = pixRead(get_path().c_str());
            m_img_cv = cv::imread(get_path().c_str());
            m_api->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
            m_api->SetImage(m_img_pix);
        }
        if (!m_api->DetectOrientationScript(&orient_deg, &orient_conf, nullptr, nullptr))
        {
            std::cerr << "Error: Could not detect the orientation of the input image" << std::endl;
        }
        else
        {
            std::cout << boost::format("orient_deg=%d, orient_conf=%.2f") % orient_deg % orient_conf << std::endl;
        }
    }

    std::string engine_tesseract::text()
    {
        return m_api->GetUTF8Text();
    }

    std::string engine_tesseract::text(const std::vector<int> &bounding_box)
    {
        set_bounding_box(bounding_box[0], bounding_box[1], bounding_box[2], bounding_box[3]);
        return m_api->GetUTF8Text();
    }

    int engine_tesseract::conf()
    {
        return m_api->MeanTextConf();
    }

    void engine_tesseract::overlay(const std::vector<std::vector<int>> &bounding_boxes)
    {
        std::string path_out = get_path();
        for (const auto &box : bounding_boxes)
        {
            const int &x = box[0], &y = box[1], &w = box[2], &h = box[3];
            cv::Rect rec = cv::Rect(x, y, w, h);
            cv::rectangle(m_img_cv, rec, cv::Scalar(0, 0, 255), 2, 8, 0);
        }
        path_out = std::regex_replace(path_out, std::regex("input"), "output");
        path_out = std::regex_replace(path_out, std::regex(".jpg"), "_overlay.jpg");
        cv::imwrite(path_out, m_img_cv);
    }

    engine_tesseract::boxx *engine_tesseract::get_bounding_boxes(int padding)
    {
        return m_api->GetComponentImages(tesseract::RIL_TEXTLINE, true, true, padding, nullptr, nullptr, nullptr);
    }

    engine_tesseract::box *engine_tesseract::get_bounding_box(engine_tesseract::boxx *bounding_boxes, int index)
    {
        engine_tesseract::box *box = boxaGetBox(bounding_boxes, index, L_CLONE);
        bounding_box_storage.push_back(box);
        return box;
    }

    void engine_tesseract::set_bounding_box(int left, int top, int width, int height)
    {
        m_api->SetRectangle(left, top, width, height);
    }

} // namespace ocr
