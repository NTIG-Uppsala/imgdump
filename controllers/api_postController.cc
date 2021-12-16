#include "api_postController.h"
#include <string>
using namespace api;
//add definition of your processing function here
void postController::uploadEndpoint(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback){
    Json::Value ret;
    MultiPartParser fileUpload;
    // Uploading multiple files at once is not allowed
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
    // Checks the file extension to see if the file is an image
    if(file.getFileType() != FileType::FT_IMAGE){
        ret["status"] = 403;
        ret["message"] = "Must be an image";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
        return;
    }

    // Format a query for uploading a file to database
    auto md5 = file.getMd5();
    auto fileUuid = drogon::utils::getUuid();
    file.saveAs(fileUuid);
    std::ostringstream databaseQuery;
    databaseQuery << "INSERT INTO `images`(`id`, `uuid`, `fileExt`, `md5`) VALUES (NULL,'" << fileUuid << "','" << fileExt << "','" << md5 << "')"; 

    // Send the query to the database
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
    // uuid's are always 32 characters long in C++
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
        // Checks the database for the uuid
        auto clientPtr = drogon::app().getDbClient();
        databaseQuery1 << "SELECT `uuid` FROM `images` WHERE uuid = '" << imageId << "';"; // Format the query
        auto sentQuery1 = clientPtr -> execSqlAsyncFuture(databaseQuery1.str(), "default"); // Send the query to the database
        try {
            auto result = sentQuery1.get(); // Hämtar resultat från query
            for (auto row : result)
            {
                exists = row["uuid"].as<std::string>();
            }
        } 
            catch (int error) {
                std::cerr << "errors:" << error << std::endl;
        }
        // If the uuid does not exist in database, send error
        if (exists == ""){
            ret["status"] = 403;
            ret["message"] = "uuid not found in database";
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            imageId = "";
            callback(resp);
        }
        // Else delete the row from the database
        databaseQuery2 << "DELETE FROM `images` WHERE uuid = '" << imageId << "';"; // Format the query
        auto sentQuery2 = clientPtr -> execSqlAsyncFuture(databaseQuery2.str(), "default"); // Send the query to the database
        ret["status"] = 200;
        ret["message"] = "Image successfully deleted";
        ret["exists printout"] = exists;
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        imageId = "";
        callback(resp);
    }
    // Send error if the input uuid is not valid
    else{
        ret["status"] = 403;
        ret["message"] = "Invalid uuid";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        imageId = "";
        callback(resp);
    }
}