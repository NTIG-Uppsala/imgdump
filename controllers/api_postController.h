#pragma once
#include <drogon/HttpController.h>
using namespace drogon;
namespace api
{
class postController:public drogon::HttpController<postController>
{
  public:
    METHOD_LIST_BEGIN
    //use METHOD_ADD to add your custom processing function here;
    //METHOD_ADD(postController::get,"/{2}/{1}",Get);//path is /api/postController/{arg2}/{arg1}
    //METHOD_ADD(postController::your_method_name,"/{1}/{2}/list",Get);//path is /api/postController/{arg1}/{arg2}/list
    //ADD_METHOD_TO(postController::your_method_name,"/absolute/path/{1}/{2}/list",Get);//path is /absolute/path/{arg1}/{arg2}/list
    ADD_METHOD_TO(postController::uploadEndpoint,"/upload_endpoint",Post); //path is /upload_endpoint
    ADD_METHOD_TO(postController::deleteEndpoint,"/delete_endpoint/{1}",Post); //path is /delete_endpoint

    METHOD_LIST_END
    // your declaration of the processing function may look like this:
    // void get(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback,int p1,std::string p2);
    // void your_method_name(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback,double p1,int p2) const;
    void uploadEndpoint(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback);
    void deleteEndpoint(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback, std::string imageId);
};
}
