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
        ret["message"] = "GIF not allowed";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
        return;
    }

    if(file.getFileType() != FileType::FT_IMAGE){
        ret["status"] = 403;
        ret["message"] = "Must be a image";
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

// This function deletes an image from database and storage, using uuid.
void postController::deleteEndpoint(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback, std::string imageId){
    Json::Value ret;
    std::ostringstream databaseQuery1;
    std::ostringstream databaseQuery2;
    bool validImageId = true;
    std::string exists;
    // uuid are always 32 characters long in C++
    if (imageId.length() != 32){
        validImageId = false;
    }
    // Checks if uuid only contains valid characters
    char acceptedChars[] = "1234567890ABCDEF";
    for(auto& elem: imageId){
        bool validChar = false;
        for(auto& character: acceptedChars) {
            if ( elem == character){
                validChar = true;
            }
        }
        if (validChar == false){
            validImageId = false;
        }
    }
    if (validImageId == true){
        auto clientPtr = drogon::app().getDbClient();
        databaseQuery1 << "SELECT `uuid` FROM `images` WHERE uuid = '" << imageId << "';"; //"SELECT EXISTS (SELECT * FROM `images` WHERE uuid = '" << imageId << "');"; //Formaterar queryn
        auto sentQuery1 = clientPtr -> execSqlAsyncFuture(databaseQuery1.str(), "default"); //Skickar en query till databasen
        try {
            auto result = sentQuery1.get(); //Hämtar resultat från query
            for (auto row : result)
            {
                exists = row["uuid"].as<std::string>();
            }
        } 
            catch (int error) {
                std::cerr << "errors:" << error << std::endl;
        }
        if (exists == ""){
            ret["status"] = 403;
            ret["message"] = "uuid not found in database";
            ret["valid image id checker"] = validImageId;
            ret["image id length"] = imageId.length();
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            imageId = "";
            callback(resp);
        }
        databaseQuery2 << "DELETE FROM `images` WHERE uuid = '" << imageId << "';"; //Formaterar queryn
        auto sentQuery2 = clientPtr -> execSqlAsyncFuture(databaseQuery2.str(), "default"); //Skickar en query till databasen
        ret["status"] = 200;
        ret["message"] = "Image successfully deleted";
        ret["exists printout"] = exists;
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        imageId = "";
        callback(resp);
    }
    else{
        ret["status"] = 403;
        ret["message"] = "Invalid uuid";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        imageId = "";
        callback(resp);
    }
}