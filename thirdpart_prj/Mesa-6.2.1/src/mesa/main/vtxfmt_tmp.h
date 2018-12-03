
/*
 * Mesa 3-D graphics library
 * Version:  4.1
 *
 * Copyright (C) 1999-2002  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Gareth Hughes
 */

#ifndef PRE_LOOPBACK
#define PRE_LOOPBACK( FUNC )
#endif

static void GLAPIENTRY TAG(ArrayElement)( GLint i )
{
   PRE_LOOPBACK( ArrayElement );
   GL_CALL(ArrayElement)( i );
}

static void GLAPIENTRY TAG(Color3f)( GLfloat r, GLfloat g, GLfloat b )
{
   PRE_LOOPBACK( Color3f );
   GL_CALL(Color3f)( r, g, b );
}

static void GLAPIENTRY TAG(Color3fv)( const GLfloat *v )
{
   PRE_LOOPBACK( Color3fv );
   GL_CALL(Color3fv)( v );
}

static void GLAPIENTRY TAG(Color4f)( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
   PRE_LOOPBACK( Color4f );
   GL_CALL(Color4f)( r, g, b, a );
}

static void GLAPIENTRY TAG(Color4fv)( const GLfloat *v )
{
   PRE_LOOPBACK( Color4fv );
   GL_CALL(Color4fv)( v );
}

static void GLAPIENTRY TAG(EdgeFlag)( GLboolean e )
{
   PRE_LOOPBACK( EdgeFlag );
   GL_CALL(EdgeFlag)( e );
}

static void GLAPIENTRY TAG(EdgeFlagv)( const GLboolean *v )
{
   PRE_LOOPBACK( EdgeFlagv );
   GL_CALL(EdgeFlagv)( v );
}

static void GLAPIENTRY TAG(EvalCoord1f)( GLfloat s )
{
   PRE_LOOPBACK( EvalCoord1f );
   GL_CALL(EvalCoord1f)( s );
}

static void GLAPIENTRY TAG(EvalCoord1fv)( const GLfloat *v )
{
   PRE_LOOPBACK( EvalCoord1fv );
   GL_CALL(EvalCoord1fv)( v );
}

static void GLAPIENTRY TAG(EvalCoord2f)( GLfloat s, GLfloat t )
{
   PRE_LOOPBACK( EvalCoord2f );
   GL_CALL(EvalCoord2f)( s, t );
}

static void GLAPIENTRY TAG(EvalCoord2fv)( const GLfloat *v )
{
   PRE_LOOPBACK( EvalCoord2fv );
   GL_CALL(EvalCoord2fv)( v );
}

static void GLAPIENTRY TAG(EvalPoint1)( GLint i )
{
   PRE_LOOPBACK( EvalPoint1 );
   GL_CALL(EvalPoint1)( i );
}

static void GLAPIENTRY TAG(EvalPoint2)( GLint i, GLint j )
{
   PRE_LOOPBACK( EvalPoint2 );
   GL_CALL(EvalPoint2)( i, j );
}

static void GLAPIENTRY TAG(FogCoordfEXT)( GLfloat f )
{
   PRE_LOOPBACK( FogCoordfEXT );
   GL_CALL(FogCoordfEXT)( f );
}

static void GLAPIENTRY TAG(FogCoordfvEXT)( const GLfloat *v )
{
   PRE_LOOPBACK( FogCoordfvEXT );
   GL_CALL(FogCoordfvEXT)( v );
}

static void GLAPIENTRY TAG(Indexf)( GLfloat f )
{
   PRE_LOOPBACK( Indexf );
   GL_CALL(Indexf)( f );
}

static void GLAPIENTRY TAG(Indexfv)( const GLfloat *v )
{
   PRE_LOOPBACK( Indexfv );
   GL_CALL(Indexfv)( v );
}

static void GLAPIENTRY TAG(Materialfv)( GLenum face, GLenum pname, const GLfloat *v )
{
   PRE_LOOPBACK( Materialfv );
   GL_CALL(Materialfv)( face, pname, v );
}

static void GLAPIENTRY TAG(MultiTexCoord1fARB)( GLenum target, GLfloat a )
{
   PRE_LOOPBACK( MultiTexCoord1fARB );
   GL_CALL(MultiTexCoord1fARB)( target, a );
}

static void GLAPIENTRY TAG(MultiTexCoord1fvARB)( GLenum target, const GLfloat *tc )
{
   PRE_LOOPBACK( MultiTexCoord1fvARB );
   GL_CALL(MultiTexCoord1fvARB)( target, tc );
}

static void GLAPIENTRY TAG(MultiTexCoord2fARB)( GLenum target, GLfloat s, GLfloat t )
{
   PRE_LOOPBACK( MultiTexCoord2fARB );
   GL_CALL(MultiTexCoord2fARB)( target, s, t );
}

static void GLAPIENTRY TAG(MultiTexCoord2fvARB)( GLenum target, const GLfloat *tc )
{
   PRE_LOOPBACK( MultiTexCoord2fvARB );
   GL_CALL(MultiTexCoord2fvARB)( target, tc );
}

static void GLAPIENTRY TAG(MultiTexCoord3fARB)( GLenum target, GLfloat s,
				     GLfloat t, GLfloat r )
{
   PRE_LOOPBACK( MultiTexCoord3fARB );
   GL_CALL(MultiTexCoord3fARB)( target, s, t, r );
}

static void GLAPIENTRY TAG(MultiTexCoord3fvARB)( GLenum target, const GLfloat *tc )
{
   PRE_LOOPBACK( MultiTexCoord3fvARB );
   GL_CALL(MultiTexCoord3fvARB)( target, tc );
}

static void GLAPIENTRY TAG(MultiTexCoord4fARB)( GLenum target, GLfloat s,
				     GLfloat t, GLfloat r, GLfloat q )
{
   PRE_LOOPBACK( MultiTexCoord4fARB );
   GL_CALL(MultiTexCoord4fARB)( target, s, t, r, q );
}

static void GLAPIENTRY TAG(MultiTexCoord4fvARB)( GLenum target, const GLfloat *tc )
{
   PRE_LOOPBACK( MultiTexCoord4fvARB );
   GL_CALL(MultiTexCoord4fvARB)( target, tc );
}

static void GLAPIENTRY TAG(Normal3f)( GLfloat x, GLfloat y, GLfloat z )
{
   PRE_LOOPBACK( Normal3f );
   GL_CALL(Normal3f)( x, y, z );
}

static void GLAPIENTRY TAG(Normal3fv)( const GLfloat *v )
{
   PRE_LOOPBACK( Normal3fv );
   GL_CALL(Normal3fv)( v );
}

static void GLAPIENTRY TAG(SecondaryColor3fEXT)( GLfloat r, GLfloat g, GLfloat b )
{
   PRE_LOOPBACK( SecondaryColor3fEXT );
   GL_CALL(SecondaryColor3fEXT)( r, g, b );
}

static void GLAPIENTRY TAG(SecondaryColor3fvEXT)( const GLfloat *v )
{
   PRE_LOOPBACK( SecondaryColor3fvEXT );
   GL_CALL(SecondaryColor3fvEXT)( v );
}

static void GLAPIENTRY TAG(TexCoord1f)( GLfloat s )
{
   PRE_LOOPBACK( TexCoord1f );
   GL_CALL(TexCoord1f)( s );
}

static void GLAPIENTRY TAG(TexCoord1fv)( const GLfloat *tc )
{
   PRE_LOOPBACK( TexCoord1fv );
   GL_CALL(TexCoord1fv)( tc );
}

static void GLAPIENTRY TAG(TexCoord2f)( GLfloat s, GLfloat t )
{
   PRE_LOOPBACK( TexCoord2f );
   GL_CALL(TexCoord2f)( s, t );
}

static void GLAPIENTRY TAG(TexCoord2fv)( const GLfloat *tc )
{
   PRE_LOOPBACK( TexCoord2fv );
   GL_CALL(TexCoord2fv)( tc );
}

static void GLAPIENTRY TAG(TexCoord3f)( GLfloat s, GLfloat t, GLfloat r )
{
   PRE_LOOPBACK( TexCoord3f );
   GL_CALL(TexCoord3f)( s, t, r );
}

static void GLAPIENTRY TAG(TexCoord3fv)( const GLfloat *tc )
{
   PRE_LOOPBACK( TexCoord3fv );
   GL_CALL(TexCoord3fv)( tc );
}

static void GLAPIENTRY TAG(TexCoord4f)( GLfloat s, GLfloat t, GLfloat r, GLfloat q )
{
   PRE_LOOPBACK( TexCoord4f );
   GL_CALL(TexCoord4f)( s, t, r, q );
}

static void GLAPIENTRY TAG(TexCoord4fv)( const GLfloat *tc )
{
   PRE_LOOPBACK( TexCoord4fv );
   GL_CALL(TexCoord4fv)( tc );
}

static void GLAPIENTRY TAG(Vertex2f)( GLfloat x, GLfloat y )
{
   PRE_LOOPBACK( Vertex2f );
   GL_CALL(Vertex2f)( x, y );
}

static void GLAPIENTRY TAG(Vertex2fv)( const GLfloat *v )
{
   PRE_LOOPBACK( Vertex2fv );
   GL_CALL(Vertex2fv)( v );
}

static void GLAPIENTRY TAG(Vertex3f)( GLfloat x, GLfloat y, GLfloat z )
{
   PRE_LOOPBACK( Vertex3f );
   GL_CALL(Vertex3f)( x, y, z );
}

static void GLAPIENTRY TAG(Vertex3fv)( const GLfloat *v )
{
   PRE_LOOPBACK( Vertex3fv );
   GL_CALL(Vertex3fv)( v );
}

static void GLAPIENTRY TAG(Vertex4f)( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   PRE_LOOPBACK( Vertex4f );
   GL_CALL(Vertex4f)( x, y, z, w );
}

static void GLAPIENTRY TAG(Vertex4fv)( const GLfloat *v )
{
   PRE_LOOPBACK( Vertex4fv );
   GL_CALL(Vertex4fv)( v );
}

static void GLAPIENTRY TAG(CallList)( GLuint i )
{
   PRE_LOOPBACK( CallList );
   GL_CALL(CallList)( i );
}

static void GLAPIENTRY TAG(CallLists)( GLsizei sz, GLenum type, const GLvoid *v )
{
   PRE_LOOPBACK( CallLists );
   GL_CALL(CallLists)( sz, type, v );
}

static void GLAPIENTRY TAG(Begin)( GLenum mode )
{
   PRE_LOOPBACK( Begin );
   GL_CALL(Begin)( mode );
}

static void GLAPIENTRY TAG(End)( void )
{
   PRE_LOOPBACK( End );
   GL_CALL(End)();
}

static void GLAPIENTRY TAG(Rectf)( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
{
   PRE_LOOPBACK( Rectf );
   GL_CALL(Rectf)( x1, y1, x2, y2 );
}

static void GLAPIENTRY TAG(DrawArrays)( GLenum mode, GLint start, GLsizei count )
{
   PRE_LOOPBACK( DrawArrays );
   GL_CALL(DrawArrays)( mode, start, count );
}

static void GLAPIENTRY TAG(DrawElements)( GLenum mode, GLsizei count, GLenum type,
			       const GLvoid *indices )
{
   PRE_LOOPBACK( DrawElements );
   GL_CALL(DrawElements)( mode, count, type, indices );
}

static void GLAPIENTRY TAG(DrawRangeElements)( GLenum mode, GLuint start,
				    GLuint end, GLsizei count,
				    GLenum type, const GLvoid *indices )
{
   PRE_LOOPBACK( DrawRangeElements );
   GL_CALL(DrawRangeElements)( mode, start, end, count, type, indices );
}

static void GLAPIENTRY TAG(EvalMesh1)( GLenum mode, GLint i1, GLint i2 )
{
   PRE_LOOPBACK( EvalMesh1 );
   GL_CALL(EvalMesh1)( mode, i1, i2 );
}

static void GLAPIENTRY TAG(EvalMesh2)( GLenum mode, GLint i1, GLint i2,
			    GLint j1, GLint j2 )
{
   PRE_LOOPBACK( EvalMesh2 );
   GL_CALL(EvalMesh2)( mode, i1, i2, j1, j2 );
}

static void GLAPIENTRY TAG(VertexAttrib1fNV)( GLuint index, GLfloat x )
{
   PRE_LOOPBACK( VertexAttrib1fNV );
   GL_CALL(VertexAttrib1fNV)( index, x );
}

static void GLAPIENTRY TAG(VertexAttrib1fvNV)( GLuint index, const GLfloat *v )
{
   PRE_LOOPBACK( VertexAttrib1fvNV );
   GL_CALL(VertexAttrib1fvNV)( index, v );
}

static void GLAPIENTRY TAG(VertexAttrib2fNV)( GLuint index, GLfloat x, GLfloat y )
{
   PRE_LOOPBACK( VertexAttrib2fNV );
   GL_CALL(VertexAttrib2fNV)( index, x, y );
}

static void GLAPIENTRY TAG(VertexAttrib2fvNV)( GLuint index, const GLfloat *v )
{
   PRE_LOOPBACK( VertexAttrib2fvNV );
   GL_CALL(VertexAttrib2fvNV)( index, v );
}

static void GLAPIENTRY TAG(VertexAttrib3fNV)( GLuint index, GLfloat x, GLfloat y, GLfloat z )
{
   PRE_LOOPBACK( VertexAttrib3fNV );
   GL_CALL(VertexAttrib3fNV)( index, x, y, z );
}

static void GLAPIENTRY TAG(VertexAttrib3fvNV)( GLuint index, const GLfloat *v )
{
   PRE_LOOPBACK( VertexAttrib3fvNV );
   GL_CALL(VertexAttrib3fvNV)( index, v );
}

static void GLAPIENTRY TAG(VertexAttrib4fNV)( GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   PRE_LOOPBACK( VertexAttrib4fNV );
   GL_CALL(VertexAttrib4fNV)( index, x, y, z, w );
}

static void GLAPIENTRY TAG(VertexAttrib4fvNV)( GLuint index, const GLfloat *v )
{
   PRE_LOOPBACK( VertexAttrib4fvNV );
   GL_CALL(VertexAttrib4fvNV)( index, v );
}


static GLvertexformat TAG(vtxfmt) = {
   TAG(ArrayElement),
   TAG(Color3f),
   TAG(Color3fv),
   TAG(Color4f),
   TAG(Color4fv),
   TAG(EdgeFlag),
   TAG(EdgeFlagv),
   TAG(EvalCoord1f),
   TAG(EvalCoord1fv),
   TAG(EvalCoord2f),
   TAG(EvalCoord2fv),
   TAG(EvalPoint1),
   TAG(EvalPoint2),
   TAG(FogCoordfEXT),
   TAG(FogCoordfvEXT),
   TAG(Indexf),
   TAG(Indexfv),
   TAG(Materialfv),
   TAG(MultiTexCoord1fARB),
   TAG(MultiTexCoord1fvARB),
   TAG(MultiTexCoord2fARB),
   TAG(MultiTexCoord2fvARB),
   TAG(MultiTexCoord3fARB),
   TAG(MultiTexCoord3fvARB),
   TAG(MultiTexCoord4fARB),
   TAG(MultiTexCoord4fvARB),
   TAG(Normal3f),
   TAG(Normal3fv),
   TAG(SecondaryColor3fEXT),
   TAG(SecondaryColor3fvEXT),
   TAG(TexCoord1f),
   TAG(TexCoord1fv),
   TAG(TexCoord2f),
   TAG(TexCoord2fv),
   TAG(TexCoord3f),
   TAG(TexCoord3fv),
   TAG(TexCoord4f),
   TAG(TexCoord4fv),
   TAG(Vertex2f),
   TAG(Vertex2fv),
   TAG(Vertex3f),
   TAG(Vertex3fv),
   TAG(Vertex4f),
   TAG(Vertex4fv),
   TAG(CallList),
   TAG(CallLists),
   TAG(Begin),
   TAG(End),
   TAG(VertexAttrib1fNV),
   TAG(VertexAttrib1fvNV),
   TAG(VertexAttrib2fNV),
   TAG(VertexAttrib2fvNV),
   TAG(VertexAttrib3fNV),
   TAG(VertexAttrib3fvNV),
   TAG(VertexAttrib4fNV),
   TAG(VertexAttrib4fvNV),
   TAG(Rectf),
   TAG(DrawArrays),
   TAG(DrawElements),
   TAG(DrawRangeElements),
   TAG(EvalMesh1),
   TAG(EvalMesh2)
};

#undef TAG
#undef PRE_LOOPBACK
