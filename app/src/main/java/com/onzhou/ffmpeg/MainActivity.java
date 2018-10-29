package com.onzhou.ffmpeg;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import com.onzhou.ffmpeg.NowStreamer;
import com.onzhou.ffmpeg.R;

public class MainActivity extends AppCompatActivity {

    private NowStreamer nowStreamer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void onInfoClick(View view) {
        if (nowStreamer == null) {
            nowStreamer = new NowStreamer();
        }
        Toast.makeText(this, "" + nowStreamer.getAvFormatInfo(), Toast.LENGTH_SHORT).show();
    }
}
