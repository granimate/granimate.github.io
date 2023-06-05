package main

import (
    "github.com/gofiber/fiber/v2"
    "fmt"
    "os/exec"
)

func main() {
    app := fiber.New()

    app.Static("/", "./public")

    app.Get("/", func(c *fiber.Ctx) error {
        return c.SendString("Hello, World!")
    })

    app.Get("/ls", func(c *fiber.Ctx) error {
        cmd := exec.Command("ls", "-l")
        out, err := cmd.CombinedOutput()
        if(err != nil) { 
            //fmt.Println("Error: ", err)
        }   

        return c.SendString(fmt.Sprintf("%s\n", out))
    })

    app.Post("/upload", func(c *fiber.Ctx) error {
        // Get first file from form field "file":
        file, err := c.FormFile("file")
        if(err != nil) {
            //log.Fatal(err)
        }

        // Save file to root directory:
        c.SaveFile(file, fmt.Sprintf("./uploads/%s", file.Filename))

        r_center := c.FormValue("r-center")
        c_center := c.FormValue("c-center")
        r_radius := c.FormValue("r-radius")
        c_radius := c.FormValue("c-radius")
        scale := c.FormValue("scale")
        no_frames := c.FormValue("no-frames")

        cmd := exec.Command("./granimate", file.Filename, r_center, r_radius, c_center, c_radius, scale, no_frames)
        err = cmd.Run()
        if(err != nil) { 
            //fmt.Println("Error: ", err)
        }

        fmt.Println(fmt.Sprintf("{\"path\": \"../mp4/%s.mp4\"}", file.Filename))

        return c.SendString(fmt.Sprintf("{\"path\": \"../mp4/%s.mp4\"}", file.Filename))
    })

    app.Listen(":80")
    //app.ListenTLS(":443", "/etc/letsencrypt/live/granimate.art/privkey.pem", "/etc/letsencrypt/live/granimate.art/fullchain.pem");
}