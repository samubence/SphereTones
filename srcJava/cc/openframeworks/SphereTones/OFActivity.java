package cc.openframeworks.SphereTones;

import java.io.File;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.StatFs;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import cc.openframeworks.OFAndroid;
import android.view.View;
import android.view.WindowManager;
import android.widget.Toast;

public class OFActivity extends cc.openframeworks.OFActivity{

	static int densityDpi;
	static Context context;
	static File lastSharedFile;
	
	@Override
    public void onCreate(Bundle savedInstanceState)
    { 
        super.onCreate(savedInstanceState);
        String packageName = getPackageName();

        runOnUiThread(new Runnable() {
			public void run() {
				getWindow().addFlags(
						WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
			}
		});     
                
        enterFullscreen();
        
        ofApp = new OFAndroid(packageName,this);
        
        context = getApplicationContext();
        
        DisplayMetrics dm = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(dm);
		densityDpi = dm.densityDpi;
    }
	
	@Override
	public void onDetachedFromWindow() {
	}
	
    @Override
    protected void onPause() {
        super.onPause();
        ofApp.pause();             
    }

    @Override
    protected void onResume() {
        super.onResume();
        ofApp.resume();
        
        enterFullscreen();
        
        if (lastSharedFile != null)
        {
        	lastSharedFile.delete();        	
        }
    }
        
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
	if (OFAndroid.keyDown(keyCode, event)) {
	    return true;
	} else {
	    return super.onKeyDown(keyCode, event);
	}
    }
    
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
	if (OFAndroid.keyUp(keyCode, event)) {
	    return true;
	} else {
	    return super.onKeyUp(keyCode, event);
	}
    }

    public void enterFullscreen()
    {
    	getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LOW_PROFILE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }

	OFAndroid ofApp;
    
	
	
    // Menus
    // http://developer.android.com/guide/topics/ui/menus.html
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    	// Create settings menu options from here, one by one or infalting an xml
        return super.onCreateOptionsMenu(menu);
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    	// This passes the menu option string to OF
    	// you can add additional behavior from java modifying this method
    	// but keep the call to OFAndroid so OF is notified of menu events
    	if(OFAndroid.menuItemSelected(item.getItemId())){
    		
    		return true;
    	}
    	return super.onOptionsItemSelected(item);
    }
    
    @Override
    public boolean onPrepareOptionsMenu (Menu menu){
    	// This method is called every time the menu is opened
    	//  you can add or remove menu options from here
    	return  super.onPrepareOptionsMenu(menu);
    }
    
    public static int getDensityDpi()
    {    
    	return densityDpi;
    }
	
    public static void shareAudio(String path, String fileName)
    {
    	Log.d("SphereTones", path + " " + fileName);
    	
    	File file = new File(path, fileName);
    	
    	Intent intent = new Intent(Intent.ACTION_SEND);
    	intent.setType("audio/*");
        
        Uri uri = Uri.parse("file://" + file);
        intent.putExtra(Intent.EXTRA_STREAM, uri);
        intent.putExtra(Intent.EXTRA_TEXT, "made width SphereTones :: www.binaura.net");        
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		
        lastSharedFile = file;
        context.startActivity(intent);
    }
    
    public static int getFreeSpace()
    {
    	int freeSpace = 1;
    	try
    	{
	    	StatFs stat = new StatFs(Environment.getExternalStorageDirectory().getPath());
	        long bytesAvailable = (long)stat.getBlockSize() * (long)stat.getAvailableBlocks();
	        freeSpace = (int)(bytesAvailable / (1024.f * 1024.f));
    	}
    	catch (Exception e)
    	{
    		
    	}    	
        return freeSpace;
    }
}



