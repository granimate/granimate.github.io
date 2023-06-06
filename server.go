package main

import (
    "github.com/gofiber/fiber/v2"
    "fmt"
    "os/exec"
    "strconv"
    "time"
)

func main() {
    app := fiber.New()

    app.Static("/", "./public")

    app.Get("/", func(c *fiber.Ctx) error {
        return c.SendString("Hello, World!")
    })

    /*app.Get("/ls", func(c *fiber.Ctx) error {
        cmd := exec.Command("ls", "-l")
        out, err := cmd.CombinedOutput()
        if(err != nil) { 
            //fmt.Println("Error: ", err)
        }   

        return c.SendString(fmt.Sprintf("%s\n", out))
    })*/

    no_started := 0
    no_finished := 0
    max_animating := 10

    app.Get("/max-id", func(c *fiber.Ctx) error {
        return c.SendString(strconv.Itoa(no_finished))
    })

    app.Post("/upload", func(c *fiber.Ctx) error {
        cid := no_started
        no_started++

        for(cid >= no_finished + max_animating) {
            time.Sleep(1*time.Second)
        }

        // Get first file from form field "file":
        file, err := c.FormFile("file")
        if(err != nil) {
            //log.Fatal(err)
        }

        // Save file to uploads directory:
        c.SaveFile(file, fmt.Sprintf("./uploads/%d", cid))
        //c.SaveFile(file, "./uploads/" + strconv.Itoa(cid))

        r_center := c.FormValue("r-center")
        c_center := c.FormValue("c-center")
        r_radius := c.FormValue("r-radius")
        c_radius := c.FormValue("c-radius")
        scale := c.FormValue("scale")
        no_frames := c.FormValue("no-frames")

        cmd := exec.Command("./granimate", strconv.Itoa(cid), r_center, r_radius, c_center, c_radius, scale, no_frames)
        err = cmd.Run()
        if(err != nil) { 
            //fmt.Println("Error: ", err)
        }

        no_finished++

        return c.SendString(fmt.Sprintf("{\"path\": \"../mp4/%d.mp4\"}", cid))
        //return c.SendString("{\"path\": \"../mp4/" + strconv.Itoa(cid) + ".mp4\"}")
    })

    //app.Listen(":80")
    app.ListenTLS(":443", "/etc/letsencrypt/live/granimate.art/fullchain.pem", "/etc/letsencrypt/live/granimate.art/privkey.pem");
}