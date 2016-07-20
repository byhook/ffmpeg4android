package ffmpeg.byhook.cn;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.Toast;

import cn.byhook.ffmpegwithx264.FFmpegPlayer;

public class MainActivity extends AppCompatActivity {

    private FFmpegPlayer mFFmpegPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();
    }

    private void initView() {
        mFFmpegPlayer = new FFmpegPlayer();
        Toast.makeText(this, "" + mFFmpegPlayer.getMessage(), Toast.LENGTH_SHORT).show();
    }
}
