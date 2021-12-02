#pragma once
#include <drogon/HttpController.h>
using namespace drogon;
namespace api
{
class getController:public drogon::HttpController<getController>
{
  public:
    METHOD_LIST_BEGIN
    //use METHOD_ADD to add your custom processing function here;
    //METHOD_ADD(getController::get,"/{2}/{1}",Get);//path is /api/getController/{arg2}/{arg1}
    //METHOD_ADD(getController::your_method_name,"/{1}/{2}/list",Get);//path is /api/getController/{arg1}/{arg2}/list
    //ADD_METHOD_TO(getController::your_method_name,"/absolute/path/{1}/{2}/list",Get);//path is /absolute/path/{arg1}/{arg2}/list
    ADD_METHOD_TO(getController::ViewTestFileUpload,"/test/sendfile",Get);//path is /test/sendfile
    ADD_METHOD_TO(getController::GetImageAndRespondToClient,"/image/{1}",Get);//path is /image/{1}

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback,int p1,std::string p2);
    // void your_method_name(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback,double p1,int p2) const;
    void ViewTestFileUpload(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback);
    void GetImageAndRespondToClient(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback, std::string imageId);
};
}
