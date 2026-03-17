package cn.wps.moffice_eng;

import android.app.Application;
import android.content.Context;

public class AppApplication extends Application {
    public static Context context;
    @Override
    public void onCreate() {
        super.onCreate();
        context = getApplicationContext();
    }
}
