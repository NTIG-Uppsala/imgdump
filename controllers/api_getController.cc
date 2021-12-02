#include "api_getController.h"
using namespace api;
//add definition of your processing function here
void getController::ViewTestFileUpload(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback){
    auto resp = HttpResponse::newHttpViewResponse("FileUpload");
    callback(resp);
}

void getController::GetImageFromDatabase(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback, std::string imageId){
    std::ostringstream databaseQuery;
    std::string fileExt;
    auto clientPtr = drogon::app().getDbClient();
    databaseQuery << "SELECT `fileExt` FROM `images` WHERE uuid = '" << imageId << "';";
    auto sentQuery = clientPtr -> execSqlAsyncFuture(databaseQuery.str(), "default");
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
    LOG_DEBUG << fileName.str(); 
    auto fileType = ContentType::CT_CUSTOM;
    if(fileExt == "jpg" || fileExt == "jpeg"){
      fileType = ContentType::CT_IMAGE_JPG;
    }
    if(fileExt == "png"){
      fileType = ContentType::CT_IMAGE_PNG;
    }
    auto resp = HttpResponse::newFileResponse(filePath.str(), fileName.str(), fileType); // ct bestämmer vilke ext
    callback(resp);
}