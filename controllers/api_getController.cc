#include "api_getController.h"
using namespace api;
// Add definition of your processing function here
void getController::ViewTestFileUpload(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback){
    auto resp = HttpResponse::newHttpViewResponse("FileUpload"); // Answers API calls with views/FileUpload.csp
    callback(resp);
}

void getController::GetImageAndRespondToClient(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback, std::string imageId){
    std::ostringstream databaseQuery;
    std::string fileExt;
    
    auto clientPtr = drogon::app().getDbClient();
    databaseQuery << "SELECT `fileExt` FROM `images` WHERE uuid = '" << imageId << "';"; // Format the query
    auto sentQuery = clientPtr -> execSqlAsyncFuture(databaseQuery.str(), "default"); // Send the query to the database
    try {
      auto result = sentQuery.get();
      for (auto row : result)
        {
            fileExt = row["fileExt"].as<std::string>();
        }
    } 
      catch (int error) {
        std::cerr << "errors:" << error << std::endl;
    }

    std::ostringstream filePath;
    filePath << "./uploads/" << imageId;

    std::ostringstream fileName;
    fileName << "Image." << fileExt;

    auto fileType = ContentType::CT_CUSTOM;
    if(fileExt == "jpg" || fileExt == "jpeg"){ // Sets ContentType Header to image/jpeg
      fileType = ContentType::CT_IMAGE_JPG;
    }
    if(fileExt == "png"){ // Sets ContentType Header to image/png
      fileType = ContentType::CT_IMAGE_PNG;
    }

    auto resp = HttpResponse::newFileResponse(filePath.str(), fileName.str(), fileType); // ContentType determines file extension
    callback(resp);
}