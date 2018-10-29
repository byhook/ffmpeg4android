package com.onzhou.ffmpeg.streamer;

import android.content.Context;
import android.os.AsyncTask;

import java.io.Closeable;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.ref.WeakReference;

public class VideoReleaseTask extends AsyncTask<Void, Void, String> {

    private WeakReference<Context> weakContext;

    private ReleaseCallback releaseCallback;

    public VideoReleaseTask(Context context, ReleaseCallback releaseCallback) {
        this.weakContext = new WeakReference<>(context);
        this.releaseCallback = releaseCallback;
    }

    @Override
    protected String doInBackground(Void... voids) {
        InputStream inputStream = null;
        FileOutputStream fos = null;
        try {
            Context appContext = weakContext.get();
            if (appContext != null) {
                inputStream = appContext.getAssets().open("input.mp4");
                File targetFile = new File(appContext.getExternalFilesDir(null), "input.mp4");
                fos = new FileOutputStream(targetFile);
                int length;
                byte[] buffer = new byte[8 * 1024];
                while ((length = inputStream.read(buffer)) != -1) {
                    fos.write(buffer, 0, length);
                    fos.flush();
                }
                return targetFile.getAbsolutePath();
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            ioClose(fos);
            ioClose(inputStream);
        }
        return null;
    }

    @Override
    protected void onPostExecute(String videoPath) {
        super.onPostExecute(videoPath);
        if (releaseCallback != null) {
            releaseCallback.onReleaseSuccess(videoPath);
        }
    }

    private void ioClose(Closeable closeable) {
        if (closeable != null) {
            try {
                closeable.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public interface ReleaseCallback {

        void onReleaseSuccess(String videoPath);

    }

}