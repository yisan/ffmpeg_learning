package com.ing.ffmpeg;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    private Button show_protocol, show_format, show_decodec, show_confuretion, show_filter,decode;
    TextView tv;

    //引入相关库
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("swscale-4");

    }

    public native String urlprotocolinfo();

    public native String avformatinfo();

    public native String avcodecinfo();

    public native String avfilterinfo();

    public native String configurationinfo();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        tv = (TextView) findViewById(R.id.sample_text);
        show_confuretion = (Button) findViewById(R.id.show_configuration);
        show_confuretion.setOnClickListener(this);
        show_decodec = (Button) findViewById(R.id.show_avcodec);
        show_decodec.setOnClickListener(this);
        show_filter = (Button) findViewById(R.id.show_filter);
        show_filter.setOnClickListener(
                this
        );
        show_format = (Button) findViewById(R.id.show_avformat);
        show_format.setOnClickListener(
                this
        );
        show_protocol = (Button) findViewById(R.id.show_protocol);
        show_protocol.setOnClickListener(this);
        decode=findViewById(R.id.decode);
        decode.setOnClickListener(this);
    }

//    /**
//     * A native method that is implemented by the 'native-lib' native library,
//     * which is packaged with this application.
//     */
//    public native String stringFromJNI();


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.show_avcodec:
                tv.setText(avcodecinfo());
                break;
            case R.id.show_avformat:
                tv.setText(avformatinfo());
                break;
            case R.id.show_configuration:
                tv.setText(configurationinfo());
                break;
            case R.id.show_filter:
                tv.setText(avfilterinfo());
                break;
            case R.id.show_protocol:
                tv.setText(urlprotocolinfo());
                break;
            case R.id.decode:
                startActivity(new Intent(this,DecodecActivity.class));
        }
    }
}
