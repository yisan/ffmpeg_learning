package com.ing.ffmpeg;

import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

/**
 * Created by ing on 2019/8/1
 */
public class DecodecActivity extends AppCompatActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_decodec);
        Button start = findViewById(R.id.start);
        final EditText edt_input = findViewById(R.id.edt_input);
        final EditText edt_output = findViewById(R.id.edt_output);
        start.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String folderUrl = Environment.getExternalStorageDirectory().getPath();
                String urlinput = folderUrl+"/"+edt_input.getText().toString();
                String urloutput = folderUrl+"/"+edt_output.getText().toString();
                Log.i("Url","input url ="+urlinput);
                Log.i("Url","output url ="+urloutput);
                decode(urlinput,urloutput);
            }
        });
    }
    //JNI
    public native int decode(String inputurl, String outputurl);
}
