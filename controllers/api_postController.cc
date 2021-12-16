#include "api_postController.h"
#include <string>
using namespace api;
// Add definition of your processing function here
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

    // Format a query for uploading a file to the database
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

// This function deletes an image from the database and storage, using UUID.
void postController::deleteEndpoint(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback, std::string imageId){
    Json::Value ret;
    std::ostringstream databaseSelectQuery;
    std::ostringstream databaseDeleteQuery;
    std::ostringstream databaseConfirmQuery;
    std::string imageIdExists;
    std::string imageIdConfirm;
    bool validImageId = true;
    // UUID's are always 32 characters long in C++
    if (imageId.length() != 32){
        validImageId = false;
    }
    // Checks if UUID only contains valid characters
    char acceptedChars[] = "1234567890ABCDEF";
    // Loops through every character of imageId
    for(auto& digit: imageId){
        bool validChar = false;
        // Loops through every character of accepted chars
        for(auto& character: acceptedChars) {
            if ( digit == character){
                validChar = true;
            }
        }
        // If a faulty character is found the imageId is invalid
        if (validChar == false){
            validImageId = false;
        }
    }
    
    if (validImageId == true){
        // Checks the database for the UUID
        auto clientPtr = drogon::app().getDbClient();
        databaseSelectQuery << "SELECT `uuid` FROM `images` WHERE uuid = '" << imageId << "';"; // Format the query
        auto sentSelectQuery = clientPtr -> execSqlAsyncFuture(databaseSelectQuery.str(), "default"); // Send the query to the database
        try {
            auto result = sentSelectQuery.get();
            for (auto row : result)
            {
                imageIdExists = row["uuid"].as<std::string>();
            }
        } 
            catch (int error) {
                std::cerr << "errors:" << error << std::endl;
        }
        // If the UUID does not exist in the database, send error
        if (imageIdExists == ""){
            ret["status"] = 403;
            ret["message"] = "uuid not found in database";
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        }
        // Else delete the row from the database
        databaseDeleteQuery << "DELETE FROM `images` WHERE uuid = '" << imageId << "';"; // Format the query
        auto sentDeleteQuery = clientPtr -> execSqlAsyncFuture(databaseDeleteQuery.str(), "default"); // Send the query to the database
        
        // Confirms that the delete query was successful 
        databaseConfirmQuery << "SELECT `uuid` FROM `images` WHERE uuid = '" << imageId << "';"; // Format the query
        auto sentConfirmQuery = clientPtr -> execSqlAsyncFuture(databaseConfirmQuery.str(), "default"); // Send the query to the database
        try {
            auto resultConfirm = sentConfirmQuery.get();
            for (auto row : resultConfirm)
            {
                imageIdConfirm = row["uuid"].as<std::string>();
            }
        } 
            catch (int error) {
                std::cerr << "errors:" << error << std::endl;
        }
        // If the uuid does not exist in database, send success message
        if (imageIdConfirm == ""){
            ret["status"] = 200;
            ret["message"] = "Image successfully deleted";
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        }
        // for edgecases where the delete query went wrong
        else{
            ret["status"] = 403;
            ret["message"] = "Something went wrong, try again";
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        }
    }
    // Send error if the input UUID is not valid
    else{
        ret["status"] = 403;
        ret["message"] = "Invalid uuid";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
}