package com.onzhou.ffmpeg.task;

import android.content.Context;
import android.os.AsyncTask;

import java.io.Closeable;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.ref.WeakReference;

public class AssertReleaseTask extends AsyncTask<Void, Void, Void> {

    private WeakReference<Context> weakContext;

    private String[] fileNames;

    private ReleaseCallback releaseCallback;

    public AssertReleaseTask(Context context, String[] fileNames, ReleaseCallback releaseCallback) {
        this.weakContext = new WeakReference<>(context);
        this.fileNames = fileNames;
        this.releaseCallback = releaseCallback;
    }

    public AssertReleaseTask(Context context, String fileName, ReleaseCallback releaseCallback) {
        this.weakContext = new WeakReference<>(context);
        this.fileNames = new String[]{fileName};
        this.releaseCallback = releaseCallback;
    }

    @Override
    protected Void doInBackground(Void... voids) {
        InputStream inputStream = null;
        FileOutputStream fos = null;
        try {
            Context appContext = weakContext.get();
            if (appContext != null) {
                for (String fileName : fileNames) {
                    inputStream = appContext.getAssets().open(fileName);
                    File targetFile = new File(appContext.getExternalFilesDir(null), fileName);
                    fos = new FileOutputStream(targetFile);
                    int length;
                    byte[] buffer = new byte[8 * 1024];
                    while ((length = inputStream.read(buffer)) != -1) {
                        fos.write(buffer, 0, length);
                        fos.flush();
                    }
                }
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
    protected void onPostExecute(Void filePath) {
        super.onPostExecute(filePath);
        if (releaseCallback != null) {
            releaseCallback.onReleaseSuccess(null);
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

        void onReleaseSuccess(String filePath);

    }

}