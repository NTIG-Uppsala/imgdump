#include "api_postController.h"
using namespace api;
//add definition of your processing function here
void postController::uploadEndpoint(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback){
    Json::Value ret;
    MultiPartParser fileUpload; // File Parser
    if (fileUpload.parse(req) != 0 || fileUpload.getFiles().size() != 1) // Är mer eller mindre än 1 fil eller storleken på filen är 0b;
    {
        ret["status"] = 403;
        ret["message"] = "Must be one file";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
        return;
    }

    auto &file = fileUpload.getFiles()[0];
    
    if(file.getFileExtension() == "gif"){ // Filen är en gif
        ret["status"] = 403;
        ret["message"] = "Gifs not allowed";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
        return;
    }

    if(file.getFileType() != FileType::FT_IMAGE){ // Filen är inte en bild
        ret["status"] = 403;
        ret["message"] = "Must be a image";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
        return;
    }

    auto md5 = file.getMd5();
    auto fileUuid = drogon::utils::getUuid();
    ret["status"] = 200;
    ret["md5"] = md5;
    ret["filename"] = fileUuid;
    ret["message"] = "Succsessful upload";
    file.saveAs(fileUuid);
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}