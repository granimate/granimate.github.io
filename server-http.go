package main

import (
    "fmt"
    "io"
    "log"
    "net/http"
    "os"
    "os/exec"
    "strconv"
    "time"
)

var no_started = 0
var no_finished = 0
var max_animating = 10

func getMaxId(w http.ResponseWriter, req *http.Request) {
    w.Header().Set("Content-Type", "text/plain")
    w.Write([]byte(strconv.Itoa(no_finished)))
    // fmt.Fprintf(w, "This is an example server.\n")
    // io.WriteString(w, "This is an example server.\n")
}

func postUpload(w http.ResponseWriter, req *http.Request) {
    cid := no_started
    no_started++

    for(cid >= no_finished + max_animating) {
        time.Sleep(1*time.Second)
    }

    req.ParseMultipartForm(10 << 20)

    //file, handler, err := req.FormFile("file")
    //fmt.Printf("Uploaded File: %+v\n", handler.Filename)
    //fmt.Printf("File Size: %+v\n", handler.Size)
    //fmt.Printf("MIME Header: %+v\n", handler.Header)

    file, _, err := req.FormFile("file")
    if err != nil {
        fmt.Println("Error Retrieving the File")
        fmt.Println(err)
        return
    }
    defer file.Close()

    f, err := os.OpenFile("./uploads/" + strconv.Itoa(cid), os.O_WRONLY|os.O_CREATE, 0666)
    defer f.Close()
    io.Copy(f, file)

    r_center  := req.PostFormValue("r-center")
    c_center  := req.PostFormValue("c-center")
    r_radius  := req.PostFormValue("r-radius")
    c_radius  := req.PostFormValue("c-radius")
    scale     := req.PostFormValue("scale")
    no_frames := req.PostFormValue("no-frames")

    cmd := exec.Command("./granimate", strconv.Itoa(cid), r_center, r_radius, c_center, c_radius, scale, no_frames)
    err = cmd.Run()
    if(err != nil) { 
        //fmt.Println("Error: ", err)
    }

    no_finished++

    w.Header().Set("Content-Type", "text/plain")
    w.Write([]byte("{\"path\": \"../mp4/" + strconv.Itoa(cid) + ".mp4\"}"))
    //fmt.Fprintf(w, "{\"path\": \"../mp4/%d.mp4\"}", cid)
}

func main() {
    https := (os.Args[2] == "https")
    domain := os.Args[3]
    port := os.Args[4]

    mux := http.NewServeMux()
    mux.HandleFunc("/max-id", getMaxId)
    mux.HandleFunc("/upload", postUpload)
    mux.Handle("/", http.StripPrefix("/", http.FileServer(http.Dir("./public"))))

    cmd := exec.Command("/bin/bash", "-c", "rm ./public/mp4/*.mp4")
    err = cmd.Run()
    if(err != nil) { 
        fmt.Println("Error: ", err)
    }

    if(https) {
        fmt.Println("hosting " + domain + " over https on port " + port)
        for {
            err := http.ListenAndServeTLS(":" + port, 
                "/etc/letsencrypt/live/" + domain + "/fullchain.pem", 
                "/etc/letsencrypt/live/" + domain + "/privkey.pem", 
                mux);
            if err != nil {
                log.Println("ListenAndServe: ", err)
            }
        }
    } else {
        fmt.Println("hosting " + domain + " over http on port " + port)
        for {
            err := http.ListenAndServe(":" + port, mux)
            if err != nil {
                log.Println("Listen: ", err)
            }
        }
    }
}