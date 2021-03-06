//
// Copyright (c) 2016 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#include "ofx/CloudPlatform/VisionRequestItem.h"
#include "ofx/IO/ByteBuffer.h"
#include "ofx/IO/Base64Encoding.h"


namespace ofx {
namespace CloudPlatform {


const std::map<VisionRequestItem::Feature::Type, std::string> VisionRequestItem::Feature::TYPE_STRINGS =
{
    { Type::TYPE_UNSPECIFIED, "TYPE_UNSPECIFIED" },
    { Type::LABEL_DETECTION, "LABEL_DETECTION" },
    { Type::TEXT_DETECTION, "TEXT_DETECTION" },
    { Type::DOCUMENT_TEXT_DETECTION, "DOCUMENT_TEXT_DETECTION" },
    { Type::FACE_DETECTION, "FACE_DETECTION" },
    { Type::LANDMARK_DETECTION, "LANDMARK_DETECTION" },
    { Type::LOGO_DETECTION, "LOGO_DETECTION" },
    { Type::SAFE_SEARCH_DETECTION, "SAFE_SEARCH_DETECTION" },
    { Type::IMAGE_PROPERTIES, "IMAGE_PROPERTIES" },
    { Type::CROP_HINTS, "CROP_HINTS" },
    { Type::WEB_DETECTION, "WEB_DETECTION" }
};


VisionRequestItem::Feature::Feature(Type type, std::size_t maxResults)
{
    _json["type"] = TYPE_STRINGS.find(type)->second;
    _json["maxResults"] = maxResults;
}


const ofJson& VisionRequestItem::Feature::json() const
{
    return _json;
}


const std::vector<VisionRequestItem::Feature> VisionRequestItem::DEFAULT_FEATURES =
{
    Feature(Feature::Type::LABEL_DETECTION),
    Feature(Feature::Type::DOCUMENT_TEXT_DETECTION),
    Feature(Feature::Type::FACE_DETECTION),
    Feature(Feature::Type::LANDMARK_DETECTION),
    Feature(Feature::Type::LOGO_DETECTION),
    Feature(Feature::Type::SAFE_SEARCH_DETECTION),
    Feature(Feature::Type::IMAGE_PROPERTIES),
    Feature(Feature::Type::CROP_HINTS),
    Feature(Feature::Type::WEB_DETECTION)
};


VisionRequestItem::VisionRequestItem()
{
}
    

VisionRequestItem::VisionRequestItem(const ofPixels& pixels,
                                     const std::vector<Feature>& features):
    VisionRequestItem(pixels,
                      OF_IMAGE_FORMAT_JPEG,
                      OF_IMAGE_QUALITY_MEDIUM,
                      features)
{
}


VisionRequestItem::VisionRequestItem(const ofPixels& pixels,
                                     ofImageFormat format,
                                     ofImageQualityType quality,
                                     const std::vector<Feature>& features)
{
    setImage(pixels, format, quality);
    setFeatures(features);
}


VisionRequestItem::VisionRequestItem(const std::string& uri,
                                     const std::vector<Feature>& features)
{
    setImage(uri);
    setFeatures(features);
}


VisionRequestItem::VisionRequestItem(const ofBuffer& buffer,
                                     const std::vector<Feature>& features)
{
    setImage(buffer);
    setFeatures(features);
}


VisionRequestItem::~VisionRequestItem()
{
}


void VisionRequestItem::setImage(const ofPixels& pixels,
                                 ofImageFormat format,
                                 ofImageQualityType quality)
{
    ofBuffer buffer;
    ofSaveImage(pixels, buffer, format, quality);
    setImage(buffer);
}


void VisionRequestItem::setImage(const std::string& uri)
{
    if (uri.substr(0, 5).compare("gs://") == 0)
    {
        _json["image"].clear();
        _json["image"]["source"]["gcs_image_uri"] = uri;
    }
    else
    {
        setImage(ofBufferFromFile(uri));
    }
}


void VisionRequestItem::setImage(const ofBuffer& buffer)
{
    _json["image"].clear();
    _json["image"]["content"] = IO::Base64Encoding::encode(IO::ByteBuffer(buffer));
}


void VisionRequestItem::addFeature(const Feature& feature)
{
    _json["features"].push_back(feature.json());
}


void VisionRequestItem::setFeatures(const std::vector<Feature>& features)
{
    _json["features"].clear();

    for (auto& feature: features)
    {
        addFeature(feature);
    }
}


void VisionRequestItem::addAllFeatures()
{
    std::vector<Feature> features;

    for (auto& feature: Feature::TYPE_STRINGS)
    {
        features.push_back(Feature(feature.first));
    }

    setFeatures(features);
}


void VisionRequestItem::setLatitudeLongitudeBounds(double minLatitude,
                                                   double minLongitude,
                                                   double maxLatitude,
                                                   double maxLongitude)
{
    _json["imageContext"]["latLongRect"] = {
        "minLatLng", {
            { "latitude", minLatitude },
            { "longitude", minLongitude }
        },
        "maxLatLng", {
            { "latitude", maxLatitude },
            { "longitude", maxLongitude }
        }
    };
}

void VisionRequestItem::setLanguageHints(const std::vector<std::string>& languages)
{
    _json["imageContext"]["languageHints"].clear();

    for (auto& language: languages)
    {
        addLanguageHint(language);
    }
}


void VisionRequestItem::addLanguageHint(const std::string& language)
{
    _json["imageContext"]["languageHints"].push_back(language);
}


const ofJson& VisionRequestItem::json() const
{
    return _json;
}


} } // namespace ofx::CloudPlatform
