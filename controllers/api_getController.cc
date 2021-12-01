#include "api_getController.h"
using namespace api;
//add definition of your processing function here
void getController::ViewTestFileUpload(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback){
    auto resp = HttpResponse::newHttpViewResponse("FileUpload");
    callback(resp);
}