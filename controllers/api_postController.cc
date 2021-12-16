#include "api_postController.h"
#include <string>
using namespace api;
//add definition of your processing function here
void postController::uploadEndpoint(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback){
    Json::Value ret;
    MultiPartParser fileUpload;
    if (fileUpload.parse(req) != 0 || fileUpload.getFiles().size() != 1)
    {
        ret["status"] = 403;
        ret["message"] = "Must be one file";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
        return;
    }

    auto &file = fileUpload.getFiles()[0];
    auto fileExt = file.getFileExtension();
    if(fileExt == "gif"){
        ret["status"] = 403;
        ret["message"] = "GIF is not allowed";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
        return;
    }

    if(file.getFileType() != FileType::FT_IMAGE){
        ret["status"] = 403;
        ret["message"] = "Must be an image";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
        return;
    }

    auto md5 = file.getMd5();
    auto fileUuid = drogon::utils::getUuid();
    file.saveAs(fileUuid);
    std::ostringstream databaseQuery;
    databaseQuery << "INSERT INTO `images`(`id`, `uuid`, `fileExt`, `md5`) VALUES (NULL,'" << fileUuid << "','" << fileExt << "','" << md5 << "')";

    auto clientPtr = drogon::app().getDbClient();
    auto sentQuery = clientPtr -> execSqlAsyncFuture(databaseQuery.str(), "default");
    try {
      auto result = sentQuery.get();
    } 
      catch (int error) {
        std::cerr << "errors:" << error << std::endl;
    }
    

    ret["status"] = 200;
    ret["md5"] = md5;
    ret["message"] = "Successful upload";
    ret["id"] = fileUuid;
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}


void postController::deleteEndpoint(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback, std::string imageId){
    Json::Value ret;
    char acceptedChars[] = "1234567890ABCDEF";
    bool validImageId = true;
    if (imageId.length() != 32){
        validImageId = false;
    }
    for(auto & elem: imageId){
        bool validChar = false;
        for(auto & character: acceptedChars) {
            if ( elem == character){
                validChar = true;
            }
        }
        if (validChar == false){
            validImageId = false;
        }
    }
    if (validImageId == false){
        ret["status"] = 403;
        ret["message"] = "Invalid uuid";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    ret["status"] = 200;
    ret["message"] = "Valid uuid!";
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}