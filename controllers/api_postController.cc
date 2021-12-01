#include "api_postController.h"
using namespace api;
//add definition of your processing function here
void postController::uploadEndpoint(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback){
    Json::Value ret;
    MultiPartParser fileUpload;
    if (fileUpload.parse(req) != 0 || fileUpload.getFiles().size() != 1)
    {
        ret["status"] = 403;
        ret["error"] = "Must be only one file";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
        return;
    }

    auto &file = fileUpload.getFiles()[0];

    if(file.getFileExtension() == "gif"){
        ret["status"] = 403;
        ret["error"] = "Gifs not allowed";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
        return;
    }

    if(file.getFileType() != FileType::FT_IMAGE){
        ret["status"] = 403;
        ret["error"] = "Must be a image";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
        return;
    }

    auto md5 = file.getMd5();
    auto fileUuid = drogon::utils::getUuid();
    ret["status"] = 200;
    ret["md5"] = md5;
    ret["filename"] = fileUuid;
    file.saveAs(fileUuid);
    LOG_INFO << "The uploaded file has been saved to the ./uploads directory with filename " + fileUuid;
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}